#pragma once
#include <enet/enet.h>
#include <string>

struct LoginInfo {
	LoginInfo();
	~LoginInfo() = default;

    ENetSocks5Info m_socks5_info;

	std::string m_address;
	uint16_t m_port;

	float m_game_version{ 4.51 };

	int32_t m_fhash{ -716928004 };
	int32_t m_hash{};
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

	uint16_t m_protocol{ 205 };
	uint16_t m_lmode{};
	uint16_t m_cbits{ 1024 };
	uint16_t m_tr{ 4322 };

	uint32_t m_total_playtime{};
    
	void reset();
};