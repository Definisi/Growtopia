#include <client/client_pool.hpp>

#include <events/registered/game_packet/state.hpp>
#include <events/registered/game_packet/call_function.hpp>
#include <events/registered/game_packet/modify_item_inventory.hpp>
#include <events/registered/game_packet/ping_request.hpp>
#include <events/registered/game_packet/send_inventory_state.hpp>
#include <events/registered/game_packet/send_map_data.hpp>
#include <events/registered/game_packet/send_tile_tree_state.hpp>
#include <events/registered/game_packet/send_tile_update_data.hpp>
#include <events/registered/game_packet/send_tile_update_data_multiple.hpp>
#include <events/registered/game_packet/set_character_state.hpp>
#include <events/registered/game_packet/tile_change_request.hpp>
#include <events/registered/game_packet/item_change_object.hpp>

#include <events/registered/track_packet/event_name.hpp>

// Global client_pool removed - now using HttpServer's m_client_pool instance

std::shared_ptr<Client> ClientPool::add(const std::string& tank_id_name, const std::string& tank_id_pass) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (auto client : m_clients) {
		std::lock_guard<std::mutex> lock(client->m_mutex);

		if (client->m_login_info.m_tank_id_name == tank_id_name)
			return nullptr;
	}

	std::shared_ptr<Client> client = std::make_shared<Client>();

	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		client->m_login_info.m_tank_id_name = tank_id_name;
		client->m_login_info.m_tank_id_pass = tank_id_pass;
	}

	for (int i = 1; i <= 5; ++i) {
		for (int j = 1; j <= 5; ++j) {
			client->m_macro.auto_farm_tile[{i, j}] = false;
		}
	}

	client->get_event_pool()->register_track("t_0", events::event_name);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_STATE, events::state);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_CALL_FUNCTION, events::call_function);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_MODIFY_ITEM_INVENTORY, events::modify_item_inventory);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_PING_REQUEST, events::ping_request);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SEND_INVENTORY_STATE, events::send_inventory_state);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SEND_MAP_DATA, events::send_map_data);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SEND_TILE_TREE_STATE, events::send_tile_tree_state);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SEND_TILE_UPDATE_DATA, events::send_tile_update_data);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE, events::send_tile_update_data_multiple);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_SET_CHARACTER_STATE, events::set_character_state);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_TILE_CHANGE_REQUEST, events::tile_change_request);
	client->get_event_pool()->register_packet(NET_GAME_PACKET_ITEM_CHANGE_OBJECT, events::item_change_object);

	m_clients.push_back(client);

	return client;
}

std::shared_ptr<Client> ClientPool::get(const std::string& tank_id_name) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (auto client : m_clients) {
		std::lock_guard<std::mutex> lock(client->m_mutex);
		if (client->m_login_info.m_tank_id_name == tank_id_name) {
			return client;
		}
	}

	return nullptr;
}

void ClientPool::remove(const std::string& tank_id_name) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::erase_if(m_clients, [&](std::shared_ptr<Client> client) {
		std::lock_guard<std::mutex> lock(client->m_mutex);
		return client->m_login_info.m_tank_id_name == tank_id_name;
		});
}

std::vector<std::shared_ptr<Client>> ClientPool::get_clients() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_clients;
}