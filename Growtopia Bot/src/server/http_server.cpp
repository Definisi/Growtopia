/*#include <server/http_server.hpp>

#include <chrono>
#include <thread>

HttpServer::HttpServer(web::uri uri) : m_listener(uri) {
	m_listener.support(web::http::methods::POST, std::bind(&HttpServer::handle_post, this, std::placeholders::_1));

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

void HttpServer::handle_post(web::http::http_request request) {
	auto path = web::uri::decode(request.relative_uri().path());

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

			body[U("success")] = web::json::value::boolean(false);
			return this->reply_bad_request(request, body);
			});
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

void HttpServer::listen() {
	m_listener.open().wait();
}*/