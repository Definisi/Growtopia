#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>

extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}
#include <proton/variant.hpp>

#include <client/client_pool.hpp>
#include <lua/events/event_context.hpp>
#include <utils/get_current_time.hpp>
#include <utils/safe_queue.hpp>

namespace lua {
	namespace api {
		extern lua_State* m_state;
		extern std::mutex m_mutex;

		void initialize(lua_State* state, std::shared_ptr<Client> client);
		void deinitialize(lua_State* state);
		void run(lua_State* state, const std::string& path);

		static Client* l_get_client(lua_State* state);
		static int l_register_event(lua_State* state);
		static int l_listen_events(lua_State* state);
		static int l_run_thread(lua_State* state);
		static int l_sleep(lua_State* state);
		static int l_unlisten_events(lua_State* state);

		namespace client {
			static int l_connect(lua_State* state);
			static int l_get_peer(lua_State* state);
			static int l_get_inventory(lua_State* state);
			static int l_get_player(lua_State* state);
			static int l_get_world(lua_State* state);
			static int l_move_toward(lua_State* state);
			static int l_place(lua_State* state);
			static int l_punch(lua_State* state);
			static int l_send_packet(lua_State* state);
			static int l_teleport(lua_State* state);
			static int l_warp(lua_State* state);
			static int l_wear(lua_State* state);
			static int l_wrench(lua_State* state);
		}
	}
}