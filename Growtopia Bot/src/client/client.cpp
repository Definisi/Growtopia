#include <client/client.hpp>

#include <iostream>

#include <proton/packet.hpp>
#include <proton/text_scanner.hpp>

#include <utils/get_current_time.hpp>

Client::Client() {
    m_login_info.m_socks5_info.ip = nullptr;
    m_login_info.m_socks5_info.auth.username = nullptr;
    m_login_info.m_socks5_info.auth.password = nullptr;
}

Client::~Client() {
    if (m_host) {
        enet_host_destroy(m_host);
        m_host = nullptr;
    }

    if (m_peer) {
        enet_peer_reset(m_peer);
        m_peer = nullptr;
    }
}

void Client::service_poll()
{
    if (!m_host || !m_peer)
        return;
    ENetEvent event;
    while (enet_host_service(m_host, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n",
                event.peer->address.host,
                event.peer->address.port);
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                event.packet->dataLength,
                event.packet->data,
                event.peer->data,
                event.channelID);
            enet_packet_destroy(event.packet);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);
            /* Reset the peer's client information. */
            event.peer->data = NULL;
        }
    }
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

void Client::send_packet(const int32_t& type, void* data, uint32_t data_size) {
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

void Client::set_socks5_info(const std::string& ip, const uint16_t port) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_login_info.m_socks5_info.ip != nullptr) {
		delete[] m_login_info.m_socks5_info.ip;
		m_login_info.m_socks5_info.ip = nullptr;
	}
	m_login_info.m_socks5_info.ip = new char[255];
	strcpy(const_cast<char*>(m_login_info.m_socks5_info.ip), ip.c_str());

	m_login_info.m_socks5_info.port = port;
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
}

bool Client::connect() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_last_connected = utils::get_current_time<std::chrono::seconds>();

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
        if (!m_login_info.request_server_data())
            return false;
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

void Client::login() {
    TextScanner text;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        text.add("tankIDName", m_login_info.m_tank_id_name);
        text.add("tankIDPass", m_login_info.m_tank_id_pass);
        text.add("requestedName", m_login_info.m_requested_name);
        text.add("f", std::to_string(m_login_info.m_f));
        text.add("protocol", std::to_string(m_login_info.m_protocol));
        text.add("game_version", std::format("{:.2f}", m_login_info.m_game_version));
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
        if (m_login_info.m_user != 0)
            text.add("user", std::to_string(m_login_info.m_user));
        if (m_login_info.m_token != 0)
            text.add("token", std::to_string(m_login_info.m_token));
        if (!m_login_info.m_uuid_token.empty() && m_login_info.m_uuid_token != "-1")
            text.add("UUIDToken", m_login_info.m_uuid_token);
        if (!m_login_info.m_door_id.empty())
            text.add("doorID", m_login_info.m_door_id);
        text.add("zf", std::to_string(m_login_info.m_zf));
        text.add("wk", m_login_info.m_wk);

        std::cout << std::format("Logging on {}...", m_login_info.m_tank_id_name) << std::endl;
    }

    this->send_packet(NET_MESSAGE_GENERIC_TEXT, text.get_all());
}