#pragma once
#include <mutex>

extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#include <lua/llimits.h>
}

struct Event {
	std::mutex m_mutex;
	lua_State* m_state;
	int m_ref;
};