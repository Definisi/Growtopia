/*
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

	void handle_post(web::http::http_request request);

	Concurrency::task<void> on_add(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_remove(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_client(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_consume(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_move(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_place(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_send(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_warp(web::http::http_request request, web::http::http_response response, web::json::value& json);
	Concurrency::task<void> on_wrench(web::http::http_request request, web::http::http_response response, web::json::value& json);
public:
	HttpServer(web::uri uri);
	~HttpServer() = default;

	std::shared_ptr<ClientPool> get_client_pool() {
		return m_client_pool;
	}

	void listen();
};*/