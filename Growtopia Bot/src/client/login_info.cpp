#include <client/login_info.hpp>

#include <algorithm>
#include <format>
#include <iostream>

#include <curl/curl.h>
#include <proton/hash.hpp>
#include <proton/text_scanner.hpp>

#include <utils/generate_random_uuid.hpp>
#include <utils/generate_random_hex.hpp>
#include <utils/generate_random_mac.hpp>
#include <utils/klv_hash.hpp>

LoginInfo::LoginInfo() {
    m_gid = utils::generate_random_uuid();
	m_mac = utils::generate_random_mac();
    m_rid = utils::generate_random_hex(32);
	std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
    m_wk = utils::generate_random_hex(32);
	std::transform(m_wk.begin(), m_wk.end(), m_wk.begin(), ::toupper);
    m_device_id = utils::generate_random_hex(16);
    std::transform(m_device_id.begin(), m_device_id.end(), m_device_id.begin(), ::toupper);
    m_hash = proton::hash(std::format("{}RT", m_device_id).c_str());
    m_hash2 = proton::hash(std::format("{}RT", m_mac).c_str());
    m_klv = utils::klv_hash(m_game_version, m_protocol, m_rid);
}

size_t LoginInfo::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	((std::string*)userdata)->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

bool LoginInfo::request_server_data() {
    CURL* curl = curl_easy_init();

    if (!curl)
        return false;

    if (m_socks5_info.ip != nullptr && (int)m_socks5_info.port > 0 && (int)m_socks5_info.port < 65536) {
        if (m_socks5_info.auth.username != nullptr && m_socks5_info.auth.password != nullptr) {
            std::string proxy_url = std::format("socks5://{}:{}@{}:{}",
                m_socks5_info.auth.username,
                m_socks5_info.auth.password,
                m_socks5_info.ip,
                m_socks5_info.port);
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url.c_str());
            curl_easy_setopt(curl, CURLOPT_SOCKS5_AUTH, (long)CURLAUTH_BASIC);
        }
        else {
            std::string proxy_url = std::format("socks5://{}:{}",
                m_socks5_info.ip,
                m_socks5_info.port);
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url.c_str());
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.growtopia1.com/growtopia/server_data.php");

    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "User-Agent: UbiServices_SDK_2017.Final.21_ANDROID64_static");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string data = std::format("version={}&platform={}&protocol={}",
        m_game_version,
        m_platform_id,
        m_protocol);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    std::string server_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &server_data);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cout << std::format("curl_easy_perform() failed: {}", curl_easy_strerror(res)) << std::endl;
        return false;
    }

    if (server_data.empty()) {
        std::cout << "A parsing error occurred while attempting to retrieve server data. The server data is empty" << std::endl;
        return false;
    }

    TextScanner scanner(server_data);
    m_address = scanner.get("server", 0);
    if (m_address.empty()) {
        std::cout << "A parsing error occurred while attempting to retrieve server data. The server IP is empty" << std::endl;
        return false;
    }

    try {
        m_port = std::stoi(scanner.get("port", 0));
    }
    catch (...) {
        std::cout << "A parsing error occurred while attempting to retrieve server data. The server port is invalid" << std::endl;
        return false;
    }

    if (m_meta.empty())
        m_meta = scanner.get("meta", 0);

    if (m_meta.empty()) {
        std::cout << "A parsing error occurred while attempting to retrieve server data. The meta is empty" << std::endl;
        return false;
    }

    return true;
}

void LoginInfo::reset() {
	m_address = "";
	m_port = 0;
	m_meta = "";

	m_gid = utils::generate_random_uuid();
	m_rid = utils::generate_random_hex(32);
	std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
    m_device_id = utils::generate_random_hex(16);
    std::transform(m_device_id.begin(), m_device_id.end(), m_device_id.begin(), ::toupper);
	m_hash = proton::hash(std::format("{}RT", m_device_id).c_str());
}