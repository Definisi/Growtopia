#pragma once
#include <cstdint>
#include <mutex>
#include <string>

#include <enet/enet.h>
extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <client/structs/login_info.hpp>
#include <events/event_pool.hpp>
#include <lua/events/event_context.hpp>
#include <player/inventory.hpp>
#include <player/player.hpp>
#include <utils/safe_queue.hpp>
#include <world/world.hpp>

class Client : public std::enable_shared_from_this<Client> {
private:
	ENetHost* m_host;
	ENetPeer* m_peer;

	uint64_t m_last_connected;

	std::shared_ptr<EventPool> m_event_pool;
public:
	Client();
	~Client();

	lua_State* m_lua_state;

	std::mutex m_mutex;
	std::atomic<bool> m_is_listening_events{ false };
	std::atomic<bool> m_is_pathfinding{ false };
	std::atomic<uint32_t> m_gem_count{ 0 };

	LoginInfo m_login_info;
	Inventory m_inventory;
	Player m_player;
	World m_world;
	SafeQueue<lua::events::EventContext> m_event_queue;

	void set_socks5_info(const std::string& ip, const uint16_t port);
	void set_socks5_info(const std::string& ip, const uint16_t port, const std::string& username, const std::string& password);

	void reset();
	bool connect();
	void disconnect();
	void send_packet(const int32_t& type, const std::string& text);
	void send_packet(int32_t type, void* game_packet, uint32_t data_size);

	bool teleport(uint32_t x, uint32_t y);
	bool move_toward(uint32_t x, uint32_t y, uint32_t delay = 200);
	void login();
	bool consume(const uint32_t& id);
	bool wear(const uint32_t& id);
	bool place(const uint32_t& x, const uint32_t& y, const uint32_t& id);
	bool punch(const uint32_t& x, const uint32_t& y);
	void warp(const std::string& name);
	bool wrench(const uint32_t& x, const uint32_t& y);

	void service_poll();

	std::shared_ptr<EventPool> get_event_pool() {
		return m_event_pool;
	}

	ENetPeer* get_peer() {
		return m_peer;
	}
};