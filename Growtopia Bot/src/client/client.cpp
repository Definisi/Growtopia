#include <client/client.hpp>

#include <iostream>
#include <format>

#include <proton/packet.hpp>
#include <proton/text_scanner.hpp>

#include <events/event_context.hpp>
#include <utils/binary_reader.hpp>
#include <utils/binary_writer.hpp>
#include <utils/pathfinder.hpp>
#include <utils/get_current_time.hpp>

Client::Client() : m_host(nullptr), m_peer(nullptr) {
	m_lua_state = luaL_newstate();

	m_login_info.m_socks5_info.ip = nullptr;
	m_login_info.m_socks5_info.auth.username = nullptr;
	m_login_info.m_socks5_info.auth.password = nullptr;

	m_event_pool = std::make_shared<EventPool>();
}

Client::~Client() {
	if (m_peer) {
		enet_peer_reset(m_peer);
		m_peer = nullptr;
	}

	if (m_host) {
		enet_host_destroy(m_host);
		m_host = nullptr;
	}

	if (m_login_info.m_socks5_info.ip != nullptr) {
		delete[] m_login_info.m_socks5_info.ip;
		m_login_info.m_socks5_info.ip = nullptr;
	}

	if (m_login_info.m_socks5_info.auth.username != nullptr) {
		delete[] m_login_info.m_socks5_info.auth.username;
		m_login_info.m_socks5_info.auth.username = nullptr;
	}

	if (m_login_info.m_socks5_info.auth.password != nullptr) {
		delete[] m_login_info.m_socks5_info.auth.password;
		m_login_info.m_socks5_info.auth.password = nullptr;
	}
}

void Client::reset() {
	m_world = World();
	m_player = Player();
	//status = BotStatus::OFFLINE;
}

std::string Client::get_status_string() {
	switch (status)
	{
	case OFFLINE:
		return "offline";
		break;
	case ONLINE:
		return "online";
		break;
	case CONNECTED:
		return "Connected to server";
		break;
	case DISCONNECTED:
		return "Disconnected";
		break;
	case SUSPENDED:
		return "Account is suspended (permanent)";
		break;
	case TEMPBAN:
		return "Account is banned (temp)";
		break;
	case IPBAN:
		return "This IP Has been banned!";
		break;
	case LOGINFAILED:
		return "Login Failed";
		break;
	case WRONGPASS:
		return "Wrong username or password";
		break;
	case CHANGESERVER:
		return "Switching Sub-Server!";
		break;
	case GOTCAPTCHA:
		return "Solving Captcha";
		break;
	case WRONGCAPTCHA:
		return "Wrong captcha";
		break;
	case ONEXIT:
		return "Exit";
		break;
	case ONWORLD:
		return "In World";
		break;
	case INVALIDEMAIL:
		return "Invalid Email, please use new account";
		break;
	case AAP:
		return "Advanced account protection verify!!";
		break;
	case MAXIPADDRESS:
		return "Unable to create new account!";
		break;
	case GUESTCAPTCHA:
		return "Guest need to verify captcha!";
		break;
	case ERCON:
		return "Error Connecting!";
	case FAILEDENTERINGWORLD:
		return "Failed to enter world. banned/invalid!";
		break;
	case UPDATE_REQUIRED:
		return "Update Required";
		break;
	case FORBIDDEN:
		return "Error while fetching the server data growtopia1";
		break;
	case FORBIDDEN1:
		return "App Data forbidden, ask to @SpeedyInWater";
		break;
	case FORBIDDEN2:
		return "Growtopia2 forbidden...";
		break;
	default:
		return "invalid status";
		break;
	}
}

void Client::set_socks5_info(const std::string& ip, const uint16_t port) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_login_info.m_socks5_info.ip != nullptr) {
		delete[] m_login_info.m_socks5_info.ip;
		m_login_info.m_socks5_info.ip = nullptr;
	}
	m_login_info.m_socks5_info.ip = new char[255];
	strcpy(const_cast<char*>(m_login_info.m_socks5_info.ip), ip.c_str());

	m_login_info.m_socks5_info.port = port;

	socks5_ip = ip;
}

void Client::set_socks5_info(const std::string& ip, const uint16_t port, const std::string& username, const std::string& password) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_login_info.m_socks5_info.ip != nullptr) {
		delete[] m_login_info.m_socks5_info.ip;
		m_login_info.m_socks5_info.ip = nullptr;
	}
	m_login_info.m_socks5_info.ip = new char[255];
	strcpy(const_cast<char*>(m_login_info.m_socks5_info.ip), ip.c_str());

	m_login_info.m_socks5_info.port = port;

	if (m_login_info.m_socks5_info.auth.username != nullptr) {
		delete[] m_login_info.m_socks5_info.auth.username;
		m_login_info.m_socks5_info.auth.username = nullptr;
	}
	m_login_info.m_socks5_info.auth.username = new char[255];
	strcpy(const_cast<char*>(m_login_info.m_socks5_info.auth.username), username.c_str());

	if (m_login_info.m_socks5_info.auth.password != nullptr) {
		delete[] m_login_info.m_socks5_info.auth.password;
		m_login_info.m_socks5_info.auth.password = nullptr;
	}
	m_login_info.m_socks5_info.auth.password = new char[255];
	strcpy(const_cast<char*>(m_login_info.m_socks5_info.auth.password), password.c_str());
	socks5_ip = ip;
}

bool Client::connect(bool reset) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (reset) {
		this->reset();
		m_login_info.reset();
	}

	if (m_host != nullptr) {
		enet_host_destroy(m_host);
		m_host = nullptr;
	}

	m_host = enet_host_create(0, 1, 2, 0, 0);

	if (!m_host) {
		std::cout << "An error occured while trying to create an ENet host" << std::endl;
		return false;
	}

	if (enet_host_compress_with_range_coder(m_host) != 0)
		return false;

	m_host->usingNewPacket = 1;
	m_host->checksum = enet_crc32;

	if (m_login_info.m_socks5_info.ip != nullptr && (int)m_login_info.m_socks5_info.port > 0 && m_login_info.m_socks5_info.port < 65536) {
		m_host->proxyInfo = m_login_info.m_socks5_info;
		m_host->usingProxy = 1;
	}

	if (!m_host)
		return false;

	if (m_login_info.m_address == "" || m_login_info.m_port == 0) {
		std::cout << "Getting server address..." << std::endl;
		if (!m_login_info.request_server_data()) {
			status = BotStatus::FORBIDDEN;
			return false;
		}
		if (!m_login_info.request_app_data()) {
			status = BotStatus::FORBIDDEN1;
			return false;
		}
		std::cout << "Located server, connecting..." << std::endl;
	}

	ENetAddress address;
	enet_address_set_host_ip(&address, m_login_info.m_address.c_str());
	address.port = m_login_info.m_port;

	m_peer = enet_host_connect(m_host, &address, 2, 0);

	m_login_info.m_address = "";
	m_login_info.m_port = 0;

	if (!m_peer)
		return false;

	enet_host_flush(m_host);
	return true;
}

void Client::disconnect() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_peer != nullptr) {
		enet_peer_reset(m_peer);
		m_peer = nullptr;
	}

	if (m_host != nullptr) {
		enet_host_destroy(m_host);
		m_host = nullptr;
	}
}

void Client::send_packet(const int32_t& type, const std::string& text) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_peer)
		return;

	if (m_peer->state != ENET_PEER_STATE_CONNECTED)
		return;

	ENetPacket* packet = enet_packet_create(nullptr, 5 + text.length(), ENET_PACKET_FLAG_RELIABLE);
	std::memcpy(packet->data, &type, 4);
	std::memcpy(packet->data + 4, text.c_str(), text.length());

	if (enet_peer_send(m_peer, 0, packet) != 0)
		enet_packet_destroy(packet);
}

void Client::send_packet(int32_t type, void* data, uint32_t data_size) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_peer)
		return;

	if (m_peer->state != ENET_PEER_STATE_CONNECTED)
		return;

	ENetPacket* packet = enet_packet_create(nullptr, 5 + data_size, ENET_PACKET_FLAG_RELIABLE);
	std::memcpy(packet->data, &type, 4);
	packet->data[data_size + 4] = 0;

	if (data)
		std::memcpy(packet->data + 4, data, data_size);

	if (enet_peer_send(m_peer, 0, packet) != 0)
		enet_packet_destroy(packet);
}

bool Client::teleport(uint32_t x, uint32_t y) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_STATE;
	game_packet.m_pos_x = x * 32 + 8;
	game_packet.m_pos_y = y * 32 + 2;

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	Vector2i pos(x * 32 + 8, y * 32 + 2);

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_player.m_pos = pos;
	}

	return true;
}

bool Client::move_toward(uint32_t x, uint32_t y, uint32_t delay) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	if (m_is_pathfinding.load(std::memory_order_relaxed))
		return false;

	std::vector<Tile*> path;

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		Pathfinder m_pathfinder(shared_from_this(), m_world.m_tiles, m_world.m_width, m_world.m_height);

		path = m_pathfinder.find(floor(m_player.m_pos.m_x / 32),
			floor(m_player.m_pos.m_y / 32),
			x,
			y);
	}

	if (path.empty()) {
		m_is_pathfinding.store(false, std::memory_order_relaxed);
		return false;
	}

	std::reverse(path.begin(), path.end());
	m_is_pathfinding.store(true, std::memory_order_relaxed);

	for (auto tile : path) {
		if (!m_is_pathfinding.load(std::memory_order_relaxed))
			return false;

		GameUpdatePacket game_packet;
		game_packet.m_type = NET_GAME_PACKET_STATE;
		game_packet.m_pos_x = tile->m_pos.m_x * 32 + 8;
		game_packet.m_pos_y = tile->m_pos.m_y * 32 + 2;

		if (m_world.m_name.empty() || m_world.m_name == "EXIT")
			return false;

		this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

		Vector2i pos(tile->m_pos.m_x * 32 + 8, tile->m_pos.m_y * 32 + 2);

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_player.m_pos = pos;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}

	m_is_pathfinding.store(false, std::memory_order_relaxed);

	return true;
}

void Client::login() {
	TextScanner text;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		text.add("tankIDName", m_login_info.m_tank_id_name);
		text.add("tankIDPass", m_login_info.m_tank_id_pass);
		text.add("requestedName", m_login_info.m_requested_name);
		text.add("f", std::to_string(m_login_info.m_f));
		text.add("protocol", std::to_string(m_login_info.m_protocol));
		text.add("game_version", m_login_info.m_game_version);
		text.add("fz", std::to_string(m_login_info.m_fz));
		text.add("lmode", std::to_string(m_login_info.m_lmode));
		text.add("cbits", std::to_string(m_login_info.m_cbits));
		text.add("player_age", std::to_string(m_login_info.m_player_age));
		text.add("GDPR", std::to_string(m_login_info.m_gdpr));
		text.add("category", m_login_info.m_category);
		text.add("totalPlaytime", std::to_string(m_login_info.m_total_playtime));
		text.add("klv", m_login_info.m_klv);
		text.add("hash2", std::to_string(m_login_info.m_hash2));
		text.add("meta", m_login_info.m_meta);
		text.add("fhash", std::to_string(m_login_info.m_fhash));
		text.add("rid", m_login_info.m_rid);
		text.add("platformID", m_login_info.m_platform_id);
		text.add("deviceVersion", std::to_string(m_login_info.m_device_version));
		text.add("country", m_login_info.m_country);
		text.add("hash", std::to_string(m_login_info.m_hash));
		text.add("mac", m_login_info.m_mac);

		if (m_login_info.m_uuid_token.length() > 3 || m_login_info.m_lmode != 0) {
			text.add("user", std::to_string(m_login_info.m_user));
			text.add("token", std::to_string(m_login_info.m_token));
			if (m_login_info.m_lmode == 3 || m_login_info.m_lmode == 2){
				text.add("doorID", m_login_info.m_door_id);
			}
			text.add("UUIDToken", m_login_info.m_uuid_token);
		}
		text.add("wk", m_login_info.m_wk);
		text.add("zf", std::to_string(m_login_info.m_zf));
		text.add("aat", std::to_string(m_login_info.m_aat));

		
		//std::cout << " Packet : \n" << text.get_all();
		//std::cout << std::format("Logging on {}...", m_login_info.m_tank_id_name) << std::endl;
	}

	this->send_packet(NET_MESSAGE_GENERIC_TEXT, text.get_all());
}

bool Client::consume(const uint32_t& id) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_TILE_CHANGE_REQUEST;
	game_packet.m_item_id = id;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		game_packet.m_pos_x = (float)m_player.m_pos.m_x;
		game_packet.m_pos_y = (float)m_player.m_pos.m_y;
	}

	game_packet.m_tile_pos_x = floor(m_player.m_pos.m_x / 32);
	game_packet.m_tile_pos_y = floor(m_player.m_pos.m_y / 32);

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	return true;
}

bool Client::wear(const uint32_t& id) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_ITEM_ACTIVATE_REQUEST;
	game_packet.m_item_id = id;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		game_packet.m_pos_x = (float)m_player.m_pos.m_x;
		game_packet.m_pos_y = (float)m_player.m_pos.m_y;
	}

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	return true;
}

bool Client::place(const uint32_t& x, const uint32_t& y, const uint32_t& id) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_TILE_CHANGE_REQUEST;
	game_packet.m_item_id = id;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		game_packet.m_pos_x = (float)m_player.m_pos.m_x;
		game_packet.m_pos_y = (float)m_player.m_pos.m_y;
	}

	game_packet.m_tile_pos_x = x;
	game_packet.m_tile_pos_y = y;

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	return true;
}

bool Client::punch(const uint32_t& x, const uint32_t& y) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_TILE_CHANGE_REQUEST;
	game_packet.m_item_id = 18;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		game_packet.m_pos_x = (float)m_player.m_pos.m_x;
		game_packet.m_pos_y = (float)m_player.m_pos.m_y;
	}

	game_packet.m_tile_pos_x = x;
	game_packet.m_tile_pos_y = y;

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	return true;
}

void Client::warp(const std::string& name) {
	TextScanner text;
	text.add("action", "join_request");
	text.add("name", name);
	text.add("invitedWorld", "0");

	this->send_packet(NET_MESSAGE_GAME_MESSAGE, text.get_all());
}

bool Client::wrench(const uint32_t& x, const uint32_t& y) {
	if (m_world.m_name.empty() || m_world.m_name == "EXIT")
		return false;

	GameUpdatePacket game_packet;
	game_packet.m_type = NET_GAME_PACKET_TILE_CHANGE_REQUEST;
	game_packet.m_item_id = 32;

	{
		std::lock_guard<std::mutex> lock(m_mutex);

		game_packet.m_pos_x = (float)m_player.m_pos.m_x;
		game_packet.m_pos_y = (float)m_player.m_pos.m_y;
	}

	game_packet.m_tile_pos_x = x;
	game_packet.m_tile_pos_y = y;

	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));

	return true;
}

void Client::collect(const uint32_t& range, bool force)
{
	if (m_world.m_floating_item_count == 0)
		return;
	for (const auto& item : m_world.m_floating_items) {
		if (m_player.m_pos.distance(item.m_pos.m_x, item.m_pos.m_y) <= range * 32) {
			GameUpdatePacket game_packet{ 0 }; 
			game_packet.m_pos_x = item.m_pos.m_x;
			game_packet.m_pos_y = item.m_pos.m_y;
			game_packet.m_type = NET_GAME_PACKET_ITEM_ACTIVATE_OBJECT_REQUEST;
			game_packet.m_object_id = item.m_drop_id_offset;
			game_packet.m_int_x = static_cast<int32_t>(item.m_pos.m_x + item.m_pos.m_y + 4.f);
			this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));
		}
	}
}

void Client::smoke()
{
	if (m_world.m_players.size() == 0)
		return;
	if (m_macro.smoke_index >= m_world.m_players.size())
		m_macro.smoke_index = 0;
	auto player = m_world.m_players[m_macro.smoke_index];
	static int x, y;
	x = player.m_pos.m_x / 32, y = player.m_pos.m_y / 32;
	GameUpdatePacket game_packet{ 0 };
	game_packet.m_type = NET_GAME_PACKET_STATE;
	game_packet.m_int_data = 2034;
	game_packet.m_int_x = x;
	game_packet.m_int_y = y;
	game_packet.m_pos_x = m_player.m_pos.m_x;
	game_packet.m_pos_y = m_player.m_pos.m_y;
	game_packet.m_flags = 144 | (1 << 10) | (1 << 11);
	this->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));
	m_macro.smoke_index++;
}

void Client::service_poll() {
	if (!m_host)
		return;

	if (!m_peer)
		return;

	ENetEvent event;

	while (enet_host_service(m_host, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "ENET_EVENT_TYPE_CONNECT" << std::endl;
			status = BotStatus::CONNECTED;
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "ENET_EVENT_TYPE_DISCONNECT" << std::endl;
			status = BotStatus::DISCONNECTED;
			std::thread([&]() {
					while (!this->connect() && m_macro.auto_reconnect) {
						this->reset();
						m_login_info.reset();
						std::this_thread::sleep_for(std::chrono::seconds(30));
					}
				}).detach();
			break;
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
		    std::cout << "ENET_EVENT_TYPE_DISCONNECT_TIMEOUT" << std::endl;

			std::thread([&]() {
					while (!this->connect() && m_macro.auto_reconnect) {
						this->reset();
						m_login_info.reset();
						std::this_thread::sleep_for(std::chrono::seconds(30));
					}
				}).detach();
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE: {
			std::cout << "Recive packet " << std::endl;
			switch (*((int32_t*)event.packet->data)) {
			case NET_MESSAGE_SERVER_HELLO: {
				this->login();
				break;
			}
			case NET_MESSAGE_GAME_PACKET: {
				if (event.packet->dataLength < sizeof(GameUpdatePacket))
					return;

				GameUpdatePacket* game_packet = reinterpret_cast<GameUpdatePacket*>(event.packet->data + sizeof(int32_t));
				uint8_t* extended_data = nullptr;

				if (game_packet->m_flags == NET_GAME_PACKET_FLAGS_EXTENDED)
					extended_data = reinterpret_cast<uint8_t*>(event.packet->data + sizeof(int32_t) + sizeof(GameUpdatePacket));

				EventContext ctx{
					.m_client = shared_from_this(),
					.m_scanner = TextScanner{},
					.m_game_packet = game_packet,
					.m_extended_data = extended_data
				};

				if (!m_event_pool->execute(NET_MESSAGE_GAME_PACKET, std::format("gp_{}", game_packet->m_type), ctx)) {
					//std::cout << std::format("Unhandled NET_MESSAGE_GAME_PACKET -> {}", game_packet->m_type) << std::endl;
				}
				break;
			}
			case NET_MESSAGE_ERROR: {
				break;
			}
			case NET_MESSAGE_TRACK: {
				TextScanner text = TextScanner(reinterpret_cast<char*>(event.packet->data + 4));

				EventContext ctx{
					.m_client = shared_from_this(),
					.m_scanner = text,
					.m_game_packet = {},
					.m_extended_data = {}
				};

				if (!m_event_pool->execute(NET_MESSAGE_TRACK, std::format("t_{}", text.get("eventType", 0)), ctx)) {
					std::cout << std::format("Unhandled NET_MESSAGE_TRACK -> {}", text.get("eventType", 0)) << std::endl;
				}
				break;
			}
			case NET_MESSAGE_GENERIC_TEXT: {
				break;
			}
			case NET_MESSAGE_GAME_MESSAGE: {
				break;
			}
			default:
				break;
			}

			enet_packet_destroy(event.packet);
			break;
		}
		default:
			break;
		}
	}

	uint64_t time = get_current_time<std::chrono::milliseconds>();
	if (m_macro.auto_collect && ((m_macro.auto_collect_last + m_macro.auto_collect_interval) < time)) {
		collect(m_macro.auto_collect_range, m_macro.auto_collect_force);
		m_macro.auto_collect_last = time;
	}
	if ((m_macro.smoke_last + 50) < time) {
		smoke();
		m_macro.auto_collect_last = time;
	}

	if ((m_macro.auto_farm_place || m_macro.auto_farm_punch)) {
		int x = m_macro.auto_farm_index % 5;
		int y = m_macro.auto_farm_index / 5;
		if (m_macro.auto_farm_tile[std::make_pair(x, y)]) {

		}
	}
}