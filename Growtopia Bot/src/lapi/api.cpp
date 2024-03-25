#include <lapi/api.hpp>

#include <iostream>

#include <utils/get_current_time.hpp>

namespace lua {
	namespace api {
		static Client* l_get_client(lua_State* state) {
			lua_getglobal(state, "_CLIENT");

			if (!lua_isuserdata(state, -1)) {
				lua_pop(state, 1);

				return nullptr;
			}

			Client** userdata = (Client**)lua_touserdata(state, -1);
			lua_pop(state, 1);

			return *userdata;
		}

		static int l_register_event(lua_State* state) {
			Client* client = l_get_client(state);

			if (client) {
				const char* name = luaL_checkstring(state, 1);
				luaL_checktype(state, 2, LUA_TFUNCTION);
				lua_setglobal(state, name);
			}

			return 0;
		}

		static int l_listen_events(lua_State* state) {
			Client* client = l_get_client(state);

			if (client) {
				int time = luaL_checkinteger(state, 1);
				uint64_t last_event = get_current_time<std::chrono::seconds>() + time;

				client->m_is_listening_events.store(true, std::memory_order_relaxed);

				while (get_current_time<std::chrono::seconds>() < last_event && client->m_is_listening_events.load(std::memory_order_relaxed) && client->get_peer()->state == ENET_PEER_STATE_CONNECTED) {
					while (get_current_time<std::chrono::seconds>() < last_event && client->m_is_listening_events.load(std::memory_order_relaxed) && client->get_peer()->state == ENET_PEER_STATE_CONNECTED && !client->m_event_queue.empty()) {
						std::unordered_map<std::string, int>::iterator it;
						events::EventContext ctx = client->m_event_queue.pop();

						lua_getglobal(state, ctx.m_name.c_str());
						if (lua_isfunction(state, -1)) {
							VariantList varlist = ctx.m_varlist;

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
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}

				client->m_is_listening_events.store(false, std::memory_order_relaxed);
				client->m_event_queue.clear();
			}

			return 0;
		}

		static int l_run_thread(lua_State* state) {
			const char* code = luaL_checkstring(state, 1);

			lua_State* new_state = luaL_newstate();

			luaL_openlibs(new_state);

			lua_getglobal(state, "_CLIENT");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "_CLIENT");

			lua_getglobal(state, "sleep");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "sleep");

			lua_getglobal(state, "connect");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "connect");

			lua_getglobal(state, "get_inventory");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "get_inventory");

			lua_getglobal(state, "get_player");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "get_player");

			lua_getglobal(state, "get_world");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "get_world");

			lua_getglobal(state, "move_toward");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "move_toward");

			lua_getglobal(state, "place");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "place");

			lua_getglobal(state, "punch");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "punch");

			lua_getglobal(state, "send_packet");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "send_packet");

			lua_getglobal(state, "teleport");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "teleport");

			lua_getglobal(state, "warp");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "warp");

			lua_getglobal(state, "wear");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "wear");

			lua_getglobal(state, "wrench");
			lua_xmove(state, new_state, 1);
			lua_setglobal(new_state, "wrench");

			std::thread([new_state, code]() {
				if (luaL_dostring(new_state, code) != LUA_OK) {
					const char* error_message = lua_tostring(new_state, -1);
					std::cerr << error_message << std::endl;
					lua_close(new_state);
				}
				}).detach();

			return 0;
		}

		static int l_sleep(lua_State* state) {
			std::this_thread::sleep_for(std::chrono::milliseconds(luaL_checkinteger(state, 1)));
			return 0;
		}

		static int l_unlisten_events(lua_State* state) {
			Client* client = l_get_client(state);

			if (client) {
				client->m_is_listening_events.store(false, std::memory_order_relaxed);
			}

			return 0;
		}

		void initialize(lua_State* state, std::shared_ptr<Client> client) {
			luaL_openlibs(state);

			Client** userdata = (Client**)lua_newuserdata(state, sizeof(Client*));
			*userdata = client.get();
			lua_setglobal(state, "_CLIENT");

			lua_register(state, "register_event", l_register_event);
			lua_register(state, "listen_events", l_listen_events);
			lua_register(state, "run_thread", l_run_thread);
			lua_register(state, "sleep", l_sleep);
			lua_register(state, "unlisten_events", l_unlisten_events);

			lua_register(state, "connect", client::l_connect);
			lua_register(state, "get_peer", client::l_get_peer);
			lua_register(state, "get_inventory", client::l_get_inventory);
			lua_register(state, "get_player", client::l_get_player);
			lua_register(state, "get_world", client::l_get_world);
			lua_register(state, "move_toward", client::l_move_toward);
			lua_register(state, "place", client::l_place);
			lua_register(state, "send_packet", client::l_send_packet);
			lua_register(state, "teleport", client::l_teleport);
			lua_register(state, "punch", client::l_punch);
			lua_register(state, "warp", client::l_warp);
			lua_register(state, "wear", client::l_wear);
			lua_register(state, "wrench", client::l_wrench);
		}

		void deinitialize(lua_State* state) {
			lua_close(state);
		}

		void run(lua_State* state, const std::string& path) {
			if (luaL_loadfile(state, path.c_str()) == LUA_OK) {
				if (lua_pcall(state, 0, LUA_MULTRET, 0) != LUA_OK) {
					const char* error_message = lua_tostring(state, -1);
					std::cerr << error_message << std::endl;
					lua_pop(state, 1);
				}
			}
			else {
				const char* error_message = lua_tostring(state, -1);
				std::cerr << error_message << std::endl;
				lua_pop(state, 1);
			}
		}

		namespace client {
			static int l_connect(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					lua_pushboolean(state, client->connect());
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_get_peer(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					std::lock_guard<std::mutex> lock(client->m_mutex);

					lua_newtable(state);

					if (client->get_peer()) {
						lua_pushliteral(state, "state");
						lua_pushinteger(state, client->get_peer()->state);
						lua_settable(state, -3);
					}
					else {
						lua_pushliteral(state, "state");
						lua_pushinteger(state, 0);
						lua_settable(state, -3);
					}
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_get_inventory(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					std::lock_guard<std::mutex> lock(client->m_mutex);

					lua_newtable(state);

					lua_pushliteral(state, "size");
					lua_pushinteger(state, client->m_inventory.m_size);
					lua_settable(state, -3);

					lua_pushcfunction(state, [](lua_State* state) -> int {
						Client* client = l_get_client(state);

						if (client) {
							std::lock_guard<std::mutex> lock(client->m_mutex);

							lua_newtable(state);

							int index = 0;
							for (const auto& item : client->m_inventory.m_items) {
								lua_pushinteger(state, ++index);

								lua_newtable(state);

								lua_pushliteral(state, "id");
								lua_pushinteger(state, item.first);
								lua_settable(state, -3);

								lua_pushliteral(state, "count");
								lua_pushinteger(state, item.second);
								lua_settable(state, -3);

								lua_settable(state, -3);
							}
						}
						else {
							lua_pushnil(state);
						}

						return 1;
						});
					lua_setfield(state, -2, "get_items");

					lua_pushcfunction(state, [](lua_State* state) -> int {
						Client* client = l_get_client(state);

						if (client) {
							int id = luaL_checkinteger(state, 2);

							std::lock_guard<std::mutex> lock(client->m_mutex);

							const auto& item = client->m_inventory.get_item(id);

							lua_newtable(state);
							
							lua_pushliteral(state, "id");
							lua_pushinteger(state, item.first);
							lua_settable(state, -3);

							lua_pushliteral(state, "count");
							lua_pushinteger(state, item.second);
							lua_settable(state, -3);
						}
						else {
							lua_pushnil(state);
						}

						return 1;
						});
					lua_setfield(state, -2, "get_item");
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_get_player(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					std::lock_guard<std::mutex> lock(client->m_mutex);

					lua_newtable(state);

					lua_pushliteral(state, "name");
					lua_pushstring(state, client->m_login_info.m_tank_id_name.c_str());
					lua_settable(state, -3);

					lua_pushliteral(state, "net_id");
					lua_pushinteger(state, client->m_player.m_net_id);
					lua_settable(state, -3);

					lua_pushliteral(state, "character_state");
					lua_pushinteger(state, client->m_player.m_character_state);
					lua_settable(state, -3);

					lua_pushliteral(state, "x");
					lua_pushinteger(state, client->m_player.m_pos.m_x);
					lua_settable(state, -3);

					lua_pushliteral(state, "y");
					lua_pushinteger(state, client->m_player.m_pos.m_y);
					lua_settable(state, -3);

					lua_pushliteral(state, "gem_count");
					lua_pushinteger(state, client->m_gem_count.load(std::memory_order_relaxed));
					lua_settable(state, -3);
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_get_world(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					lua_newtable(state);

					lua_pushliteral(state, "name");
					{
						std::lock_guard<std::mutex> lock(client->m_mutex);
						lua_pushstring(state, client->m_world.m_name.c_str());
					}
					lua_settable(state, -3);

					lua_pushcfunction(state, [](lua_State* state) -> int {
						Client* client = l_get_client(state);

						if (client) {
							std::lock_guard<std::mutex> lock(client->m_mutex);

							lua_newtable(state);

							int index = 0;
							for (const auto& tile : client->m_world.m_tiles) {
								lua_pushinteger(state, ++index);

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

								lua_pushliteral(state, "is_ready");
								lua_pushboolean(state, tile.is_ready());
								lua_settable(state, -3);

								lua_settable(state, -3);
							}
						}
						else {
							lua_pushnil(state);
						}

						return 1;
						});
					lua_setfield(state, -2, "get_tiles");

					lua_pushcfunction(state, [](lua_State* state) -> int {
						Client* client = l_get_client(state);

						if (client) {
							int x = luaL_checkinteger(state, 2);
							int y = luaL_checkinteger(state, 3);

							Tile* tile = nullptr;
							{
								std::lock_guard<std::mutex> lock(client->m_mutex);
								tile = client->m_world.get_tile(x, y);
							}

							if (tile) {
								lua_newtable(state);

								lua_pushliteral(state, "x");
								lua_pushinteger(state, tile->m_pos.m_x);
								lua_settable(state, -3);

								lua_pushliteral(state, "y");
								lua_pushinteger(state, tile->m_pos.m_y);
								lua_settable(state, -3);

								lua_pushliteral(state, "foreground");
								lua_pushinteger(state, tile->m_foreground);
								lua_settable(state, -3);

								lua_pushliteral(state, "background");
								lua_pushinteger(state, tile->m_background);
								lua_settable(state, -3);

								lua_pushliteral(state, "is_ready");
								lua_pushboolean(state, tile->is_ready());
								lua_settable(state, -3);
							}
							else {
								lua_newtable(state);

								lua_pushliteral(state, "x");
								lua_pushinteger(state, x);
								lua_settable(state, -3);

								lua_pushliteral(state, "y");
								lua_pushinteger(state, y);
								lua_settable(state, -3);

								lua_pushliteral(state, "foreground");
								lua_pushinteger(state, 0);
								lua_settable(state, -3);

								lua_pushliteral(state, "background");
								lua_pushinteger(state, 0);
								lua_settable(state, -3);

								lua_pushliteral(state, "is_ready");
								lua_pushboolean(state, false);
								lua_settable(state, -3);
							}
						}
						else {
							lua_pushnil(state);
						}

						return 1;
						});
					lua_setfield(state, -2, "get_tile");
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_move_toward(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int x = luaL_checkinteger(state, 1);
					int y = luaL_checkinteger(state, 2);
					int delay = luaL_optinteger(state, 3, 200);

					lua_pushboolean(state, client->move_toward(x, y, delay));
				}
				else {
					lua_pushnil(state);
				}

				return 1;
			}

			static int l_place(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int x = luaL_checkinteger(state, 1);
					int y = luaL_checkinteger(state, 2);
					int id = luaL_checkinteger(state, 3);

					client->place(x, y, id);
				}

				return 0;
			}

			static int l_punch(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int x = luaL_checkinteger(state, 1);
					int y = luaL_checkinteger(state, 2);

					client->punch(x, y);
				}

				return 0;
			}

			static int l_send_packet(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int type = luaL_checkinteger(state, 1);
					const char* packet = luaL_checkstring(state, 2);

					client->send_packet(type, packet);
				}

				return 0;
			}

			static int l_teleport(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int x = luaL_checkinteger(state, 1);
					int y = luaL_checkinteger(state, 2);

					client->teleport(x, y);
				}

				return 0;
			}

			static int l_warp(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					const char* name = luaL_checkstring(state, 1);

					client->warp(name);
				}

				return 0;
			}

			static int l_wear(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int id = luaL_checkinteger(state, 1);

					client->wear(id);
				}

				return 0;
			}

			static int l_wrench(lua_State* state) {
				Client* client = l_get_client(state);

				if (client) {
					int x = luaL_checkinteger(state, 1);
					int y = luaL_checkinteger(state, 2);

					client->wrench(x, y);
				}

				return 0;
			}
		}
	}
}