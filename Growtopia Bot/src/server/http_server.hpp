#pragma once
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <client/client_pool.hpp>

class HttpServer {
private:
	web::http::experimental::listener::http_listener m_listener;

	std::shared_ptr<ClientPool> m_client_pool;

	Concurrency::task<void> reply_ok(web::http::http_request request, const web::json::value& body);
	Concurrency::task<void> reply_bad_request(web::http::http_request request, const web::json::value& body);
	Concurrency::task<void> reply_not_found(web::http::http_request request, const web::json::value& body);

	void handle_post(web::http::http_request request);
	void handle_get(web::http::http_request request);

	// Existing endpoints
	Concurrency::task<void> on_add(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_remove(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_client(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_consume(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_move(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_place(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_send(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_warp(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_wrench(web::http::http_request request, web::http::http_response response, web::json::value& json);

	// New useful endpoints
	Concurrency::task<void> on_list_clients(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_disconnect(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_reconnect(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_inventory(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_wear(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_punch(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_collect(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_teleport(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_status(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_ping(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_world_info(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_player_info(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_smoke(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_reset(web::http::http_request request, web::http::http_response response, web::json::value& json);
    
    // Additional information endpoints for web interface
    Concurrency::task<void> on_get_gems(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_level(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_position(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_world_players(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_world_objects(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_connection_info(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_bot_stats(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_server_info(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_item_info(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_world_tiles(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_nearby_players(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_bot_events(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_performance_stats(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_macro_status(web::http::http_request request, web::http::http_response response, web::json::value& json);
    Concurrency::task<void> on_get_pathfinding_status(web::http::http_request request, web::http::http_response response, web::json::value& json);
public:
	HttpServer(web::uri uri);
	~HttpServer() = default;

	std::shared_ptr<ClientPool> get_client_pool() {
		return m_client_pool;
	}

	void listen();
};