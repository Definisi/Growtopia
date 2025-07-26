#include <server/http_server.hpp>

#include <chrono>
#include <thread>

HttpServer::HttpServer(web::uri uri) : m_listener(uri) {
	m_listener.support(web::http::methods::POST, std::bind(&HttpServer::handle_post, this, std::placeholders::_1));
	m_listener.support(web::http::methods::GET, std::bind(&HttpServer::handle_get, this, std::placeholders::_1));

	m_client_pool = std::make_shared<ClientPool>();
}

Concurrency::task<void> HttpServer::reply_ok(web::http::http_request request, const web::json::value& body) {
	web::http::http_response response;
	response.headers().set_content_type(U("application/json"));
	response.set_status_code(web::http::status_codes::OK);
	response.set_body(body);
	return request.reply(response);
}

Concurrency::task<void> HttpServer::reply_bad_request(web::http::http_request request, const web::json::value& body) {
	web::http::http_response response;
	response.headers().set_content_type(U("application/json"));
	response.set_status_code(web::http::status_codes::BadRequest);
	response.set_body(body);
	return request.reply(response);
}

Concurrency::task<void> HttpServer::reply_not_found(web::http::http_request request, const web::json::value& body) {
	web::http::http_response response;
	response.headers().set_content_type(U("application/json"));
	response.set_status_code(web::http::status_codes::NotFound);
	response.set_body(body);
	return request.reply(response);
}

void HttpServer::handle_post(web::http::http_request request) {
	auto path = web::uri::decode(request.relative_uri().path());
	std::cout << "[" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] POST request received: " + utility::conversions::to_utf8string(path) << std::endl;
	if (path == U("/api/1.0")) {
		request.extract_json().then([=](web::json::value json) {
			web::json::value body = web::json::value::object();
			web::http::http_response response;
			response.headers().set_content_type(U("application/json"));

			if (!json.has_field(U("option")) || !json[U("option")].is_string()) {
				body[U("success")] = web::json::value::boolean(false);
				return this->reply_bad_request(request, body);
			}

			const utility::string_t& option = json[U("option")].as_string();

			if (option == U("add")) {
				return this->on_add(request, response, json);
			}
			else if (option == U("remove")) {
				return this->on_remove(request, response, json);
			}
			else if (option == U("client")) {
				return this->on_client(request, response, json);
			}
			else if (option == U("consume")) {
				return this->on_consume(request, response, json);
			}
			else if (option == U("move")) {
				return this->on_move(request, response, json);
			}
			else if (option == U("place")) {
				return this->on_place(request, response, json);
			}
			else if (option == U("send")) {
				return this->on_send(request, response, json);
			}
			else if (option == U("warp")) {
				return this->on_warp(request, response, json);
			}
			else if (option == U("wrench")) {
				return this->on_wrench(request, response, json);
			}
			// New endpoints
			else if (option == U("list_clients")) {
				return this->on_list_clients(request, response, json);
			}
			else if (option == U("disconnect")) {
				return this->on_disconnect(request, response, json);
			}
			else if (option == U("reconnect")) {
				return this->on_reconnect(request, response, json);
			}
			else if (option == U("inventory")) {
				return this->on_inventory(request, response, json);
			}
			else if (option == U("wear")) {
				return this->on_wear(request, response, json);
			}
			else if (option == U("punch")) {
				return this->on_punch(request, response, json);
			}
			else if (option == U("collect")) {
				return this->on_collect(request, response, json);
			}
			else if (option == U("teleport")) {
				return this->on_teleport(request, response, json);
			}
			else if (option == U("status")) {
				return this->on_status(request, response, json);
			}
			else if (option == U("ping")) {
				return this->on_ping(request, response, json);
			}
			else if (option == U("world_info")) {
				return this->on_world_info(request, response, json);
			}
			else if (option == U("player_info")) {
				return this->on_player_info(request, response, json);
			}
			else if (option == U("smoke")) {
				return this->on_smoke(request, response, json);
			}
			else if (option == U("reset")) {
				return this->on_reset(request, response, json);
			}
			// Additional information endpoints
			else if (option == U("get_gems")) {
				return this->on_get_gems(request, response, json);
			}
			else if (option == U("get_level")) {
				return this->on_get_level(request, response, json);
			}
			else if (option == U("get_position")) {
				return this->on_get_position(request, response, json);
			}
			else if (option == U("get_world_players")) {
				return this->on_get_world_players(request, response, json);
			}
			else if (option == U("get_world_objects")) {
				return this->on_get_world_objects(request, response, json);
			}
			else if (option == U("get_connection_info")) {
				return this->on_get_connection_info(request, response, json);
			}
			else if (option == U("get_bot_stats")) {
				return this->on_get_bot_stats(request, response, json);
			}
			else if (option == U("get_server_info")) {
				return this->on_get_server_info(request, response, json);
			}
			else if (option == U("get_item_info")) {
				return this->on_get_item_info(request, response, json);
			}
			else if (option == U("get_world_tiles")) {
				return this->on_get_world_tiles(request, response, json);
			}
			else if (option == U("get_nearby_players")) {
				return this->on_get_nearby_players(request, response, json);
			}
			else if (option == U("get_bot_events")) {
				return this->on_get_bot_events(request, response, json);
			}
			else if (option == U("get_performance_stats")) {
				return this->on_get_performance_stats(request, response, json);
			}
			else if (option == U("get_macro_status")) {
				return this->on_get_macro_status(request, response, json);
			}
			else if (option == U("get_pathfinding_status")) {
				return this->on_get_pathfinding_status(request, response, json);
			}

			body[U("success")] = web::json::value::boolean(false);
			return this->reply_bad_request(request, body);
			});
	} else {
		// Handle POST requests to other endpoints with 404 JSON response
		web::json::value body = web::json::value::object();
		body[U("success")] = web::json::value::boolean(false);
		body[U("error")] = web::json::value::string(U("Endpoint not found"));
		body[U("message")] = web::json::value::string(U("The requested endpoint does not exist. Use /api/1.0 for API requests."));
		body[U("timestamp")] = web::json::value::number(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		this->reply_not_found(request, body);
	}
}

void HttpServer::handle_get(web::http::http_request request) {
	auto path = web::uri::decode(request.relative_uri().path());
	std::cout << "[" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] GET request received: " + utility::conversions::to_utf8string(path) << std::endl;

	if (path == U("/ok")) {
		web::json::value body = web::json::value::object();
		body[U("status")] = web::json::value::string(U("OK"));
		body[U("message")] = web::json::value::string(U("Server is running"));
		body[U("timestamp")] = web::json::value::number(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		this->reply_ok(request, body);
	}
	else {
		web::json::value body = web::json::value::object();
		body[U("success")] = web::json::value::boolean(false);
		body[U("error")] = web::json::value::string(U("Endpoint not found"));
		body[U("message")] = web::json::value::string(U("The requested endpoint does not exist"));
		body[U("timestamp")] = web::json::value::number(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		this->reply_not_found(request, body);
	}
}

Concurrency::task<void> HttpServer::on_add(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("tank_id_pass")) || !json[U("tank_id_pass")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const std::string& tank_id_pass = utility::conversions::to_utf8string(json[U("tank_id_pass")].as_string());

	std::string ip;
	int port;
	std::string username;
	std::string password;

	if (json.has_field(U("socks5")) && json[U("socks5")].is_object()) {
		web::json::value socks5 = json[U("socks5")];

		if (!socks5.has_field(U("ip")) || !socks5[U("ip")].is_string() ||
			!socks5.has_field(U("port")) || !socks5[U("port")].is_integer()) {
			body[U("success")] = web::json::value::boolean(false);
			return this->reply_bad_request(request, body);
		}

		ip = utility::conversions::to_utf8string(socks5[U("ip")].as_string());
		port = socks5[U("port")].as_integer();

		if (socks5.has_field(U("username")) && socks5[U("username")].is_string() &&
			socks5.has_field(U("password")) && socks5[U("password")].is_string()) {
			username = utility::conversions::to_utf8string(socks5[U("username")].as_string());
			password = utility::conversions::to_utf8string(socks5[U("password")].as_string());
		}
	}

	std::shared_ptr<Client> client = m_client_pool->add(tank_id_name, tank_id_pass);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!ip.empty() && port > 0 && port < 65536) {
		if (!username.empty() && !password.empty()) {
			client->set_socks5_info(ip, port, username, password);
		}
		else {
			client->set_socks5_info(ip, port);
		}
	}

	if (!client->connect()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_remove(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	m_client_pool->remove(tank_id_name);

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_client(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);

	body[U("info")] = web::json::value::object();

	{
		std::lock_guard<std::mutex> lock(client->m_mutex);

		body[U("info")][U("x")] = client->m_player.m_pos.m_x;
		body[U("info")][U("y")] = client->m_player.m_pos.m_y;
		body[U("info")][U("world")] = web::json::value::string(utility::conversions::to_string_t(client->m_world.m_name));
	}

	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_consume(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("item_id")) || !json[U("item_id")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& item_id = json[U("item_id")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->consume(item_id)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_move(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("x")) || !json[U("x")].is_integer() ||
		!json.has_field(U("y")) || !json[U("y")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& x = json[U("x")].as_integer();
	const int& y = json[U("y")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->move_toward(x, y)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_send(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("type")) || !json[U("type")].is_integer() ||
		!json.has_field(U("packet")) || !json[U("packet")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& type = json[U("type")].as_integer();
	const std::string& packet = utility::conversions::to_utf8string(json[U("packet")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->send_packet(type, packet);

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_place(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("x")) || !json[U("x")].is_integer() ||
		!json.has_field(U("y")) || !json[U("y")].is_integer() ||
		!json.has_field(U("item_id")) || !json[U("item_id")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& x = json[U("x")].as_integer();
	const int& y = json[U("y")].as_integer();
	const int& item_id = json[U("item_id")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->place(x, y, item_id)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_warp(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("name")) || !json[U("name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const std::string& name = utility::conversions::to_utf8string(json[U("name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->warp(name);

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_wrench(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("x")) || !json[U("x")].is_integer() ||
		!json.has_field(U("y")) || !json[U("y")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& x = json[U("x")].as_integer();
	const int& y = json[U("y")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->wrench(x, y)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

// New endpoint implementations
Concurrency::task<void> HttpServer::on_list_clients(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();
	web::json::value clients_array = web::json::value::array();

	auto clients = m_client_pool->get_clients();
	for (size_t i = 0; i < clients.size(); ++i) {
		auto client = clients[i];
		web::json::value client_info = web::json::value::object();
		
		{
			std::lock_guard<std::mutex> lock(client->m_mutex);
			client_info[U("tank_id_name")] = web::json::value::string(utility::conversions::to_string_t(client->m_login_info.m_tank_id_name));
			client_info[U("status")] = web::json::value::string(utility::conversions::to_string_t(client->get_status_string()));
			client_info[U("world")] = web::json::value::string(utility::conversions::to_string_t(client->m_world.m_name));
			client_info[U("x")] = client->m_player.m_pos.m_x;
			client_info[U("y")] = client->m_player.m_pos.m_y;
			client_info[U("level")] = client->m_level.load();
			client_info[U("gems")] = client->m_gem_count.load();
			client_info[U("ping")] = client->get_ping();
		}
		
		clients_array[i] = client_info;
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("clients")] = clients_array;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_disconnect(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->disconnect();

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_reconnect(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->connect(true)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_inventory(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value inventory_array = web::json::value::array();
	
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		int slot = 0;
		for (const auto& item : client->m_inventory.m_items) {
			if (item.first > 0) {
				web::json::value item_info = web::json::value::object();
				item_info[U("item_id")] = item.first;
				item_info[U("count")] = item.second;
				item_info[U("slot")] = slot++;
				inventory_array[inventory_array.size()] = item_info;
			}
		}
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("inventory")] = inventory_array;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_wear(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("item_id")) || !json[U("item_id")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& item_id = json[U("item_id")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->wear(item_id)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_punch(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("x")) || !json[U("x")].is_integer() ||
		!json.has_field(U("y")) || !json[U("y")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& x = json[U("x")].as_integer();
	const int& y = json[U("y")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->punch(x, y)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_collect(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	uint32_t range = 3; // default range
	bool force = false; // default force

	if (json.has_field(U("range")) && json[U("range")].is_integer()) {
		range = json[U("range")].as_integer();
	}

	if (json.has_field(U("force")) && json[U("force")].is_boolean()) {
		force = json[U("force")].as_bool();
	}

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->collect(range, force);

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_teleport(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string() ||
		!json.has_field(U("x")) || !json[U("x")].is_integer() ||
		!json.has_field(U("y")) || !json[U("y")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	const int& x = json[U("x")].as_integer();
	const int& y = json[U("y")].as_integer();

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	if (!client->teleport(x, y)) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_status(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("status")] = web::json::value::string(utility::conversions::to_string_t(client->get_status_string()));
	body[U("status_code")] = static_cast<int>(client->status);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_ping(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("ping")] = client->get_ping();
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_world_info(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value world_info = web::json::value::object();
	
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		world_info[U("name")] = web::json::value::string(utility::conversions::to_string_t(client->m_world.m_name));
		world_info[U("width")] = client->m_world.m_width;
		world_info[U("height")] = client->m_world.m_height;
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("world")] = world_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_player_info(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value player_info = web::json::value::object();
	
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		player_info[U("tank_id_name")] = web::json::value::string(utility::conversions::to_string_t(tank_id_name));
		player_info[U("x")] = client->m_player.m_pos.m_x;
		player_info[U("y")] = client->m_player.m_pos.m_y;
		player_info[U("level")] = client->m_level.load();
		player_info[U("gems")] = client->m_gem_count.load();
		player_info[U("user_id")] = client->m_player.m_user_id;
		player_info[U("net_id")] = client->m_player.m_net_id;
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("player")] = player_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_smoke(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->smoke();

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_reset(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	client->reset();

	body[U("success")] = web::json::value::boolean(true);
	return this->reply_ok(request, body);
}

// Additional information endpoints implementations
Concurrency::task<void> HttpServer::on_get_gems(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("gems")] = client->m_gem_count.load();
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_level(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("level")] = client->m_level.load();
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_position(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value position = web::json::value::object();
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		position[U("x")] = client->m_player.m_pos.m_x;
		position[U("y")] = client->m_player.m_pos.m_y;
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("position")] = position;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_world_players(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value players_array = web::json::value::array();
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		for (const auto& player : client->m_world.m_players) {
			web::json::value player_obj = web::json::value::object();
			player_obj[U("user_id")] = player.m_user_id;
			player_obj[U("net_id")] = player.m_net_id;
			player_obj[U("x")] = player.m_pos.m_x;
			player_obj[U("y")] = player.m_pos.m_y;
			players_array[players_array.size()] = player_obj;
		}
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("players")] = players_array;
	body[U("count")] = static_cast<int>(players_array.size());
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_world_objects(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value objects_array = web::json::value::array();
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		// World objects not available in current World structure
		// Using floating items instead
		for (const auto& item : client->m_world.m_floating_items) {
			web::json::value obj = web::json::value::object();
			obj[U("item_id")] = item.m_item_id;
			obj[U("amount")] = item.m_amount;
			obj[U("x")] = item.m_pos.m_x;
			obj[U("y")] = item.m_pos.m_y;
			obj[U("drop_id")] = item.m_drop_id_offset;
			objects_array[objects_array.size()] = obj;
		}
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("objects")] = objects_array;
	body[U("count")] = static_cast<int>(objects_array.size());
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_connection_info(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value connection_info = web::json::value::object();
	// Note: host and last_connected are private members, using alternative info
	connection_info[U("status")] = web::json::value::string(utility::conversions::to_string_t(client->get_status_string()));
	connection_info[U("world")] = web::json::value::string(utility::conversions::to_string_t(client->m_world.m_name));
	connection_info[U("ping")] = client->get_ping();

	body[U("success")] = web::json::value::boolean(true);
	body[U("connection")] = connection_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_bot_stats(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value stats = web::json::value::object();
	stats[U("gems")] = client->m_gem_count.load();
	stats[U("level")] = client->m_level.load();
	stats[U("is_listening_events")] = client->m_is_listening_events.load();
	stats[U("is_pathfinding")] = client->m_is_pathfinding.load();
	stats[U("status")] = web::json::value::string(utility::conversions::to_string_t(client->get_status_string()));
	stats[U("ping")] = client->get_ping();

	body[U("success")] = web::json::value::boolean(true);
	body[U("stats")] = stats;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_server_info(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	web::json::value server_info = web::json::value::object();
	server_info[U("total_clients")] = static_cast<int>(m_client_pool->get_clients().size());
	server_info[U("server_status")] = web::json::value::string(U("running"));
	server_info[U("timestamp")] = web::json::value::number(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	body[U("success")] = web::json::value::boolean(true);
	body[U("server")] = server_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_item_info(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("item_id")) || !json[U("item_id")].is_integer()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const int& item_id = json[U("item_id")].as_integer();

	// Basic item info (you can expand this based on your item system)
	web::json::value item_info = web::json::value::object();
	item_info[U("item_id")] = item_id;
	item_info[U("name")] = web::json::value::string(U("Unknown Item"));
	item_info[U("rarity")] = 1;
	item_info[U("category")] = web::json::value::string(U("block"));

	body[U("success")] = web::json::value::boolean(true);
	body[U("item")] = item_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_world_tiles(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	// Get optional parameters for area
	int start_x = json.has_field(U("start_x")) ? json[U("start_x")].as_integer() : 0;
	int start_y = json.has_field(U("start_y")) ? json[U("start_y")].as_integer() : 0;
	int end_x = json.has_field(U("end_x")) ? json[U("end_x")].as_integer() : 10;
	int end_y = json.has_field(U("end_y")) ? json[U("end_y")].as_integer() : 10;

	web::json::value tiles_array = web::json::value::array();
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		for (int x = start_x; x <= end_x && x < client->m_world.m_width; x++) {
			for (int y = start_y; y <= end_y && y < client->m_world.m_height; y++) {
				if (x >= 0 && y >= 0) {
					auto tile = client->m_world.get_tile(x, y);
					if (tile) {
						web::json::value tile_obj = web::json::value::object();
						tile_obj[U("x")] = x;
						tile_obj[U("y")] = y;
						tile_obj[U("fg")] = tile->m_foreground;
						tile_obj[U("bg")] = tile->m_background;
						tiles_array[tiles_array.size()] = tile_obj;
					}
				}
			}
		}
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("tiles")] = tiles_array;
	body[U("area")] = web::json::value::object();
	body[U("area")][U("start_x")] = start_x;
	body[U("area")][U("start_y")] = start_y;
	body[U("area")][U("end_x")] = end_x;
	body[U("area")][U("end_y")] = end_y;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_nearby_players(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());
	int range = json.has_field(U("range")) ? json[U("range")].as_integer() : 5;

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value nearby_players = web::json::value::array();
	{
		std::lock_guard<std::mutex> lock(client->m_mutex);
		int bot_x = client->m_player.m_pos.m_x;
		int bot_y = client->m_player.m_pos.m_y;

		for (const auto& player : client->m_world.m_players) {
			int distance = abs(player.m_pos.m_x - bot_x) + abs(player.m_pos.m_y - bot_y);
			if (distance <= range) {
				web::json::value player_obj = web::json::value::object();
				player_obj[U("x")] = player.m_pos.m_x;
				player_obj[U("y")] = player.m_pos.m_y;
				player_obj[U("distance")] = distance;
				player_obj[U("user_id")] = player.m_user_id;
				player_obj[U("net_id")] = player.m_net_id;
				nearby_players[nearby_players.size()] = player_obj;
			}
		}
	}

	body[U("success")] = web::json::value::boolean(true);
	body[U("nearby_players")] = nearby_players;
	body[U("range")] = range;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_bot_events(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value events_info = web::json::value::object();
	events_info[U("is_listening")] = client->m_is_listening_events.load();
	// Note: SafeQueue doesn't have size() method, using alternative
	events_info[U("event_queue_empty")] = client->m_event_queue.empty();

	body[U("success")] = web::json::value::boolean(true);
	body[U("events")] = events_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_performance_stats(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value performance = web::json::value::object();
	performance[U("ping")] = client->get_ping();
	// Note: last_connected is private, using alternative info
	performance[U("world")] = web::json::value::string(utility::conversions::to_string_t(client->m_world.m_name));
	performance[U("status")] = web::json::value::string(utility::conversions::to_string_t(client->get_status_string()));
	// Note: SafeQueue doesn't have size() method, using alternative
	performance[U("event_queue_empty")] = client->m_event_queue.empty();

	body[U("success")] = web::json::value::boolean(true);
	body[U("performance")] = performance;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_macro_status(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value macro_info = web::json::value::object();
	macro_info[U("is_running")] = false; // Default, you can implement actual macro status
	macro_info[U("current_action")] = web::json::value::string(U("none"));

	body[U("success")] = web::json::value::boolean(true);
	body[U("macro")] = macro_info;
	return this->reply_ok(request, body);
}

Concurrency::task<void> HttpServer::on_get_pathfinding_status(web::http::http_request request, web::http::http_response response, web::json::value& json) {
	web::json::value body = web::json::value::object();

	if (!json.has_field(U("tank_id_name")) || !json[U("tank_id_name")].is_string()) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_bad_request(request, body);
	}

	const std::string& tank_id_name = utility::conversions::to_utf8string(json[U("tank_id_name")].as_string());

	std::shared_ptr<Client> client = m_client_pool->get(tank_id_name);

	if (!client) {
		body[U("success")] = web::json::value::boolean(false);
		return this->reply_ok(request, body);
	}

	web::json::value pathfinding_info = web::json::value::object();
	pathfinding_info[U("is_pathfinding")] = client->m_is_pathfinding.load();
	pathfinding_info[U("target_x")] = 0; // You can implement actual target coordinates
	pathfinding_info[U("target_y")] = 0;

	body[U("success")] = web::json::value::boolean(true);
	body[U("pathfinding")] = pathfinding_info;
	return this->reply_ok(request, body);
}

void HttpServer::listen() {
	m_listener.open().wait();
}