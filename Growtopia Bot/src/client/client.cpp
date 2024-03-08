#include <client/client.hpp>

#include <iostream>

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
}