#include <api/api.hpp>

#include <iostream>

Api::Api()
	: m_state(luaL_newstate()) {
	luaL_openlibs(m_state);

	lua_pushlightuserdata(m_state, this);
	lua_pushcclosure(m_state, [](lua_State* state) {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->add_client(state);
		}, 1);
	lua_setglobal(m_state, "add_client");

	lua_pushlightuserdata(m_state, this);
	lua_pushcclosure(m_state, [](lua_State* state) {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->sleep(state);
		}, 1);
	lua_setglobal(m_state, "sleep");
}

Api::~Api() {
	lua_close(m_state);
}

int Api::add_client(lua_State* state) {
	std::lock_guard<std::mutex> lock(m_mutex);

	const char* tank_id_name = luaL_checkstring(state, 1);
	const char* tank_id_pass = luaL_checkstring(state, 2);

	const char* ip = nullptr;
	int port = 0;
	const char* username = nullptr;
	const char* password = nullptr;

	if (lua_gettop(state) == 3 && lua_istable(state, 3)) {
		lua_getfield(state, 3, "ip");
		ip = luaL_optstring(state, -1, "ip");
		lua_pop(state, 1);

		lua_getfield(state, 3, "port");
		port = static_cast<int>(luaL_optnumber(state, -1, 0));
		lua_pop(state, 1);

		lua_getfield(state, 3, "username");
		username = luaL_optstring(state, -1, nullptr);
		lua_pop(state, 1);

		lua_getfield(state, 3, "password");
		password = luaL_optstring(state, -1, nullptr);
		lua_pop(state, 1);
	}

	std::shared_ptr<Client> client = client_pool->add(tank_id_name, tank_id_pass);

	if (!client) {
		lua_pushnil(state);
		return 1;
	}

	if (ip != nullptr && strlen(ip) != 0 && port > 0 && port < 65536) {
		if (username != nullptr && password != nullptr) {
			client->set_socks5_info(ip, port, username, password);
		}
		else {
			client->set_socks5_info(ip, port);
		}
	}

	Client** user_data = static_cast<Client**>(lua_newuserdata(state, sizeof(Client*)));
	*user_data = client.get();

	luaL_newmetatable(state, "_CLIENT");

	lua_pushvalue(state, -1);
	lua_setfield(state, -2, "__index");

	lua_pushcfunction(state, [](lua_State* state) -> int {
		Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));
		if (*user_data) {
			std::lock_guard<std::mutex> lock((*user_data)->m_mutex);
			lua_pushstring(state, (*user_data)->m_login_info.m_tank_id_name.c_str());
		}
		else {
			lua_pushnil(state);
		}

		return 1;
		});
	lua_setfield(state, -2, "get_name");

	lua_pushlightuserdata(state, this);
	lua_pushcclosure(state, [](lua_State* state) -> int {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->connect(state);
		}, 1);
	lua_setfield(state, -2, "connect");

	lua_pushlightuserdata(state, this);
	lua_pushcclosure(state, [](lua_State* state) -> int {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->get_world(state);
		}, 1);
	lua_setfield(state, -2, "get_world");

	lua_pushlightuserdata(state, this);
	lua_pushcclosure(state, [](lua_State* state) -> int {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->warp(state);
		}, 1);
	lua_setfield(state, -2, "warp");

	lua_pushlightuserdata(state, this);
	lua_pushcclosure(state, [](lua_State* state) -> int {
		Api* api = reinterpret_cast<Api*>(lua_touserdata(state, lua_upvalueindex(1)));
		return api->register_event(state);
		}, 1);
	lua_setfield(state, -2, "register_event");

	lua_setmetatable(state, -2);

	return 1;
}

void Api::execute_event(const std::string& tank_id_name, const std::string& name, VariantList varlist) {
	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_events.find(tank_id_name + "_" + name);
	if (it != m_events.end()) {
		std::lock_guard<std::mutex> event_lock(it->second->m_mutex);

		int ref = it->second->m_ref;
		lua_State* state = it->second->m_state;

		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

		if (name == "NET_GAME_PACKET_CALL_FUNCTION") {
			lua_newtable(state);

			for (uint8_t index = 0; index < C_MAX_VARIANT_LIST_PARMS; ++index) {
				lua_pushinteger(state, index);

				switch (varlist[index].get_type()) {
				case Variant::VariantType::FLOAT: {
					lua_pushnumber(state, varlist[index].get<float>());
					break;
				}
				case Variant::VariantType::STRING: {
					lua_pushstring(state, varlist[index].get<std::string>().c_str());
					break;
				}
				case Variant::VariantType::VECTOR2: {
					lua_newtable(state);

					lua_pushliteral(state, "x");
					lua_pushnumber(state, varlist[index].get<Vector2>().m_x);
					lua_settable(state, -3);

					lua_pushliteral(state, "y");
					lua_pushnumber(state, varlist[index].get<Vector2>().m_y);
					lua_settable(state, -3);
					break;
				}
				case Variant::VariantType::VECTOR3: {
					lua_newtable(state);

					lua_pushliteral(state, "x");
					lua_pushnumber(state, varlist[index].get<Vector3>().m_x);
					lua_settable(state, -3);

					lua_pushliteral(state, "y");
					lua_pushnumber(state, varlist[index].get<Vector3>().m_y);
					lua_settable(state, -3);

					lua_pushliteral(state, "z");
					lua_pushnumber(state, varlist[index].get<Vector3>().m_z);
					lua_settable(state, -3);
					break;
				}
				case Variant::VariantType::UINT: {
					lua_pushinteger(state, varlist[index].get<uint32_t>());
					break;
				}
				case Variant::VariantType::INT: {
					lua_pushinteger(state, varlist[index].get<int32_t>());
					break;
				}
				default:
					lua_pushnil(state);
					break;
				}

				lua_settable(state, -3);
			}

			if (lua_pcall(state, 1, 0, 0) != 0) {
				const char* error_message = lua_tostring(state, -1);
				std::cerr << error_message << std::endl;
				lua_pop(state, 1);
			}
		}

		lua_pop(state, lua_gettop(state));
	}
}

int Api::sleep(lua_State* state) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::this_thread::sleep_for(std::chrono::milliseconds(luaL_checkinteger(state, 1)));
	return 0;
}

int Api::connect(lua_State* state) {
	std::lock_guard<std::mutex> lock(m_mutex);

	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		lua_pushboolean(state, (*user_data)->connect());
	}
	else {
		lua_pushnil(state);
	}

	return 1;
}

int Api::get_name(lua_State* state) {
	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		std::lock_guard<std::mutex> lock((*user_data)->m_mutex);

		lua_pushstring(state, (*user_data)->m_login_info.m_tank_id_name.c_str());
	}
	else {
		lua_pushnil(state);
	}

	return 1;
}

int Api::get_world(lua_State* state) {
	std::lock_guard<std::mutex> lock(m_mutex);

	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		std::lock_guard<std::mutex> guard((*user_data)->m_mutex);

		lua_newtable(state);

		lua_pushliteral(state, "name");
		lua_pushstring(state, (*user_data)->m_world.m_name.c_str());
		lua_settable(state, -3);

		lua_pushliteral(state, "tiles");
		lua_newtable(state);
		for (uint32_t index = 0; index < (*user_data)->m_world.m_tiles.size(); ++index) {
			const Tile& tile = (*user_data)->m_world.m_tiles[index];

			lua_pushinteger(state, index);

			lua_newtable(state);

			lua_pushliteral(state, "x");
			lua_pushinteger(state, tile.m_pos.m_x);
			lua_settable(state, -3);

			lua_pushliteral(state, "y");
			lua_pushinteger(state, tile.m_pos.m_y);
			lua_settable(state, -3);

			lua_pushliteral(state, "foreground");
			lua_pushinteger(state, tile.m_foreground);
			lua_settable(state, -3);

			lua_pushliteral(state, "background");
			lua_pushinteger(state, tile.m_background);
			lua_settable(state, -3);

			lua_settable(state, -3);
		}
		lua_settable(state, -3);
	}
	else {
		lua_pushnil(state);
	}

	return 1;
}

int Api::place(lua_State* state) {
	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		const int& x = luaL_checkinteger(state, 2);
		const int& y = luaL_checkinteger(state, 3);
		const int& id = luaL_checkinteger(state, 4);

		(*user_data)->place(x, y, id);
	}

	return 0;
}

int Api::punch(lua_State* state) {
	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		const int& x = luaL_checkinteger(state, 2);
		const int& y = luaL_checkinteger(state, 3);

		(*user_data)->punch(x, y);
	}

	return 0;
}

int Api::register_event(lua_State* state) {
	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		std::lock_guard<std::mutex> lock((*user_data)->m_mutex);

		const char* name = luaL_checkstring(state, 2);
		luaL_checktype(state, 3, LUA_TFUNCTION);

		std::string event_name = std::string((*user_data)->m_login_info.m_tank_id_name) + "_" + std::string(name);
		int ref = luaL_ref(state, LUA_REGISTRYINDEX);
		lua_State* event_state = lua_newthread(state);

		m_events[event_name] = std::make_shared<Event>();
		m_events[event_name]->m_state = event_state;
		//luaL_openlibs(m_events[event_name]->m_state);
		m_events[event_name]->m_ref = ref;
	}

	return 0;
}

int Api::warp(lua_State* state) {
	std::lock_guard<std::mutex> lock(m_mutex);

	Client** user_data = static_cast<Client**>(luaL_checkudata(state, 1, "_CLIENT"));

	if (*user_data) {
		const char* name = luaL_checkstring(state, 2);

		(*user_data)->warp(name);
	}

	return 0;
}

void Api::run(const char* path) {
	if (luaL_loadfile(m_state, path) == LUA_OK) {
		if (lua_pcall(m_state, 0, LUA_MULTRET, 0) != LUA_OK) {
			const char* error_message = lua_tostring(m_state, -1);
			std::cerr << error_message << std::endl;
			lua_pop(m_state, 1);
		}
	}
	else {
		const char* error_message = lua_tostring(m_state, -1);
		std::cerr << error_message << std::endl;
		lua_pop(m_state, 1);
	}
}

Api* api = new Api();