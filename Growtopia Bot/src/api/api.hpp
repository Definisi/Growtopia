#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>

#include <api/structs/event.hpp>
#include <proton/variant.hpp>

#include <client/client_pool.hpp>

class Api {
private:
	lua_State* m_state;
	
	std::unordered_map<std::string, std::shared_ptr<Event>> m_events;

	int add_client(lua_State* state);
	int sleep(lua_State* state);

	int connect(lua_State* state);
	static int get_name(lua_State* state);
	int get_world(lua_State* state);
	static int place(lua_State* state);
	static int punch(lua_State* state);
	int register_event(lua_State* state);
	int warp(lua_State* state);
public:
	std::mutex m_mutex;

	Api();
	~Api();

	void execute_event(const std::string& tank_id_name, const std::string& name, VariantList varlist);
	void run(const char* path);
};

extern Api* api;