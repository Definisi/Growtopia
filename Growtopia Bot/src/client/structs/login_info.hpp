#pragma once
#include <cstdint>
#include <mutex>
#include <string>

#include <enet/enet.h>

struct LoginInfo {
private:
	std::string m_server_data;
	std::mutex m_callback_mutex; // Add mutex for thread safety

	static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
		if (!userdata || !ptr) return 0;
		try {
			return static_cast<LoginInfo*>(userdata)->write_callback_impl(ptr, size, nmemb);
		}
		catch (...) {
			return 0; // Return 0 to indicate error
		}
	}
	size_t write_callback_impl(char* ptr, size_t size, size_t nmemb);
public:
	LoginInfo();
	~LoginInfo() = default;

	std::string m_address;
	uint16_t m_port;

	ENetSocks5Info m_socks5_info;

	std::string m_game_version = "5.23";

	int32_t m_fhash{ -716928004 };
	int32_t m_hash{ -964334282 };
	int32_t m_token{};
	int32_t m_user{};
	int32_t m_fz{ 41380888 };
	int32_t m_zf{ 371936056 };

	std::string m_tank_id_name{};
	std::string m_tank_id_pass{};
	std::string m_requested_name{ "SickleDuck" };
	std::string m_category{ "_-5100" };
	std::string m_klv{};
	std::string m_meta{};
	std::string m_rid{};
	std::string m_platform_id{ "0,1,1" };
	std::string m_country{ "id" };
	std::string m_mac{ "02:00:00:00:00:00" };
	std::string m_wk{ "NONE0" };
	std::string m_uuid_token{};
	std::string m_door_id{};
	std::string m_login_form{}; // For Get ltoken
	std::string m_ltoken{};

	uint8_t m_f{ 1 };
	uint8_t m_player_age{ 18 };
	uint8_t m_gdpr{ 1 };
	uint8_t m_device_version{ 0 };

	uint16_t m_hash2{};
	uint16_t m_protocol{ 216 };
	uint16_t m_lmode{ 0 };
	uint16_t m_aat{ 2 };
	uint16_t m_cbits{ 1024 };

	uint32_t m_total_playtime{};

	bool request_server_data();
	bool request_app_data();

	void reset();
};