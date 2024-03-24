#pragma once
#include <cstdint>
#include <mutex>
#include <string>

#include <enet/enet.h>

struct LoginInfo {
private:
	std::string m_server_data;

	static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
		return static_cast<LoginInfo*>(userdata)->write_callback_impl(ptr, size, nmemb);
	}
	size_t write_callback_impl(char* ptr, size_t size, size_t nmemb);
public:
	LoginInfo();
	~LoginInfo() = default;

	std::string m_address;
	uint16_t m_port;

	ENetSocks5Info m_socks5_info;

	float m_game_version{ 3.99 };

	int32_t m_fhash{ -716928004 };
	int32_t m_hash{ -964334282 };
	int32_t m_token{};
	int32_t m_user{};

	std::string m_tank_id_name{};
	std::string m_tank_id_pass{};
	std::string m_requested_name{ "SickleDuck" };
	std::string m_category{ "_-5100" };
	std::string m_klv{};
	std::string m_gid{};
	std::string m_meta{};
	std::string m_rid{};
	std::string m_country{ "id" };
	std::string m_mac{ "02:00:00:00:00:00" };
	std::string m_wk{ "NONE0" };
	std::string m_uuid_token{};
	std::string m_door_id{};

	uint8_t m_f{ 1 };
	uint8_t m_player_age{ 18 };
	uint8_t m_gdpr{ 1 };
	uint8_t m_platform_id{ 4 };
	uint8_t m_device_version{};

	uint16_t m_protocol{ 173 };
	uint16_t m_lmode{};
	uint16_t m_cbits{ 1024 };
	uint16_t m_tr{ 4322 };

	uint32_t m_total_playtime{};

	bool request_server_data();

	void reset();
};