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
#include <utils/generate_rid.hpp>

LoginInfo::LoginInfo() {
	m_lmode = 0;
	m_user = 0;
	m_token = 0;
	m_door_id = "";
	m_rid = generate_rid();
	m_wk = generate_random_hex(32);
	m_hash = hash_str(std::to_string(random(100000, 250000)) + "RT");
	m_mac = generate_mac();
	m_hash2 = hash_str(m_mac + "RT");


	// m_rid is already uppercase from generate_rid()
	std::transform(m_wk.begin(), m_wk.end(), m_wk.begin(), ::toupper);

	m_klv = generate_klv(m_game_version, m_protocol, m_rid);
}

size_t LoginInfo::write_callback_impl(char* ptr, size_t size, size_t nmemb) {
	if (!ptr || size == 0 || nmemb == 0) {
		return 0;
	}

	try {
		std::lock_guard<std::mutex> lock(m_callback_mutex);
		size_t total_size = size * nmemb;
		
		// Prevent excessive memory usage
		if (m_server_data.size() + total_size > 1024 * 1024) { // 1MB limit
			std::cout << "Server response too large, truncating" << std::endl;
			return 0;
		}
		
		m_server_data.append(ptr, total_size);
		return total_size;
	}
	catch (const std::exception& e) {
		std::cout << "Exception in write_callback_impl: " << e.what() << std::endl;
		return 0;
	}
	catch (...) {
		std::cout << "Unknown exception in write_callback_impl" << std::endl;
		return 0;
	}
}

bool LoginInfo::request_server_data() {
	CURL* curl = curl_easy_init();

	if (!curl) {
		std::cout << "Failed to initialize CURL" << std::endl;
		return false;
	}

	// Clear previous server data
	m_server_data.clear();

	try {
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
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Add timeout
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L); // Add connection timeout

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
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

		CURLcode res = curl_easy_perform(curl);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			std::cout << std::format("curl_easy_perform() failed: {} ({})", curl_easy_strerror(res), static_cast<int>(res)) << std::endl;
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
			std::cout << "Server response: " << m_server_data << std::endl;
			return false;
		}

		try {
			std::string port_str = scanner.get("port", 0);
			if (port_str.empty()) {
				std::cout << "A parsing error occurred while attempting to retrieve server data. The server port is empty" << std::endl;
				std::cout << "Server response: " << m_server_data << std::endl;
				return false;
			}
			m_port = std::stoi(port_str);
		}
		catch (const std::exception& e) {
			std::cout << "A parsing error occurred while attempting to retrieve server data. The server port is invalid: " << e.what() << std::endl;
			std::cout << "Server response: " << m_server_data << std::endl;
			return false;
		}

		if (m_meta.empty())
			m_meta = scanner.get("meta", 0);

		if (m_meta.empty()) {
			std::cout << "A parsing error occurred while attempting to retrieve server data. The meta is empty" << std::endl;
			std::cout << "Server response: " << m_server_data << std::endl;
			return false;
		}

		std::cout << "Successfully retrieved server data: " << m_address << ":" << m_port << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Exception in request_server_data(): " << e.what() << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
	catch (...) {
		std::cout << "Unknown exception in request_server_data()" << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
}

bool LoginInfo::request_app_data()
{
	CURL* curl = curl_easy_init();
	CURLcode res;
	std::string result;

	if (!curl) {
		std::cout << "Failed to initialize CURL for app data" << std::endl;
		return false;
	}

	try {
		curl_easy_setopt(curl, CURLOPT_URL, "https://cdn.growpai.site/speedy/app_data.txt");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Add timeout
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L); // Add connection timeout
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			std::cout << std::format("curl_easy_perform() failed for app data: {} ({})", curl_easy_strerror(res), static_cast<int>(res)) << std::endl;
			return false;
		}

		if (result.empty()) {
			std::cout << "App data response is empty" << std::endl;
			return false;
		}

		TextScanner scanner(result);
		std::string version_str = scanner.get("game_version", 0);
		std::string protocol_str = scanner.get("protocol", 0);

		if (version_str.empty() || protocol_str.empty()) {
			std::cout << "Failed to parse app data: version or protocol is empty" << std::endl;
			std::cout << "App data response: " << result << std::endl;
			return false;
		}

		try {
			m_game_version = version_str;
			m_protocol = std::stoi(protocol_str);
		}
		catch (const std::exception& e) {
			std::cout << "Failed to parse protocol number: " << e.what() << std::endl;
			return false;
		}

		m_klv = generate_klv(m_game_version, m_protocol, m_rid);
		std::cout << "Successfully retrieved app data: version=" << m_game_version << ", protocol=" << m_protocol << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Exception in request_app_data(): " << e.what() << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
	catch (...) {
		std::cout << "Unknown exception in request_app_data()" << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
}

void LoginInfo::reset() {
	m_address = "";
	m_port = 0;
	m_meta = "";

	m_lmode = 0;
	m_user = 0;
	m_token = 0;
	m_door_id = "";
	m_rid = generate_rid();
	m_wk = generate_random_hex(32);
	m_hash = hash_str(std::to_string(random(100000, 250000)) + "RT");
	m_mac = generate_mac();
	m_hash2 = hash_str(m_mac + "RT");


	// m_rid is already uppercase from generate_rid()
	std::transform(m_wk.begin(), m_wk.end(), m_wk.begin(), ::toupper);

	m_klv = generate_klv(m_game_version, m_protocol, m_rid);
}