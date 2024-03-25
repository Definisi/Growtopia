#include <chrono>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <lapi/api.hpp>
#include <item/item_core.hpp>
 


int main() {
	if (!item_database->initialize("items.dat"))
		return EXIT_FAILURE;

	if (enet_initialize() != 0) {
		std::cout << "An error occured while initializing ENet" << std::endl;
		return EXIT_FAILURE;
	}
	std::shared_ptr<Client> client = client_pool->add("tyo", "tyo");

	for (auto client : client_pool->get_clients()) {
		lua::api::initialize(client->m_lua_state, client);
		std::thread(&lua::api::run, client->m_lua_state, "main.lua").detach();
	}

	while (1) {
		for (auto client : client_pool->get_clients()) {
			client->service_poll();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	for (auto client : client_pool->get_clients()) {
		lua::api::deinitialize(client->m_lua_state);
	}

	return EXIT_SUCCESS;
}
