#include <chrono>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <lapi/api.hpp>
#include <item/item_core.hpp>
#include <server/http_server.hpp>

#include <utils/string_split.hpp>
#include <utils/discord.hpp>
#include <utils/http_get.hpp>
#include <utils/containsignorecase.hpp>
#include <utils/generate_rid.hpp>

#include <enet/enet.h>
#include <cpprest/http_listener.h>

std::string discord_uid;
std::string discord_name;

int main() {
    std::cout << generate_rid() << std::endl;
    init_discord_rpc();
    if (!item_database->initialize("items.dat"))
        return EXIT_FAILURE;

    if (enet_initialize() != 0) {
        std::cout << "An error occured while initializing ENet" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Starting Growtopia Bot HTTP API Server..." << std::endl;


    // Initialize HTTP server on port 8080
    web::uri_builder uri(U("http://localhost"));
    uri.set_port(8080);
    uri.set_path(U("/"));

    HttpServer server(uri.to_uri());
    std::cout << "HTTP API Server listening on http://localhost:8080" << std::endl;
    std::cout << "API Endpoint: POST /api/1.0" << std::endl;
    std::cout << "Available options: add, remove, client, consume, move, place, send, warp, wrench" << std::endl;

    try {
        server.listen();
        std::cout << "Server started successfully!" << std::endl;

        // Keep the server running and handle client polling
        while (true) {
            for (auto client : server.get_client_pool()->get_clients()) {
                client->service_poll();
            }
            Discord_RunCallbacks();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }

    for (auto client : server.get_client_pool()->get_clients()) {
        //lua::api::deinitialize(client->m_lua_state);
    }
    enet_deinitialize();
    return EXIT_SUCCESS;
}