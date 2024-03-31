#include <client/structs/login_info.hpp>

#include <algorithm>
#include <format>
#include <iostream>

#include <curl/curl.h>
#include <proton/hash.hpp>
#include <proton/text_scanner.hpp>

#include <utils/generate_random_uuid.hpp>
#include <utils/random.hpp>
#include <utils/generate_mac.hpp>
#include <utils/generate_klv.hpp>
#include <utils/hash_str.hpp>

LoginInfo::LoginInfo() {
	m_lmode = 0;
	m_user = 0;
	m_token = 0;
	m_door_id = "";
	m_rid = generate_random_hex(32);
	m_wk = generate_random_hex(32);
	m_hash = hash_str(std::to_string(random(100000, 250000)) + "RT");
	m_mac = generate_mac();
	m_hash2 = hash_str(m_mac + "RT");


	std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
	std::transform(m_wk.begin(), m_wk.end(), m_wk.begin(), ::toupper);

	m_klv = generate_klv(m_game_version, m_protocol, m_hash, m_rid);
}

size_t LoginInfo::write_callback_impl(char* ptr, size_t size, size_t nmemb) {
	size_t total_size = size * nmemb;
	m_server_data.append(ptr, total_size);
	return total_size;
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

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

	CURLcode res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);

	if (res != CURLE_OK) {
		std::cout << std::format("curl_easy_perform() failed: {}", curl_easy_strerror(res)) << std::endl;
		return false;
	}

	if (m_server_data.empty()) {
		std::cout << "A parsing error occurred while attempting to retrieve server data. The server data is empty" << std::endl;
		return false;
	}

	TextScanner scanner(m_server_data);
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

bool LoginInfo::request_app_data()
{
	CURL* curl;
	CURLcode res;
	std::string result;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://cdn.growpai.site/speedy/app_data.txt");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (result.empty()) {
			return false;
		}
		TextScanner scanner(result);
		std::string version_str = scanner.get("game_version", 0);
		std::string protocol_str = scanner.get("protocol", 0);
		m_game_version = version_str;
		m_protocol = std::stoi(protocol_str);
	}
	m_klv = generate_klv(m_game_version, m_protocol, m_hash, m_rid);
	return true;
}

void LoginInfo::reset() {
	m_address = "";
	m_port = 0;
	m_meta = "";

	m_lmode = 0;
	m_user = 0;
	m_token = 0;
	m_door_id = "";
	m_rid = generate_random_hex(32);
	m_wk = generate_random_hex(32);
	m_hash = hash_str(std::to_string(random(100000, 250000)) + "RT");
	m_mac = generate_mac();
	m_hash2 = hash_str(m_mac + "RT");


	std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
	std::transform(m_wk.begin(), m_wk.end(), m_wk.begin(), ::toupper);

	m_klv = generate_klv(m_game_version, m_protocol, m_hash, m_rid);
}