#pragma once
#include <cstdint>

#include <proton/packet.hpp>
#include <proton/text_scanner.hpp>

#include <client/client.hpp>

class Client;

struct EventContext {
	std::shared_ptr<Client> m_client;
	TextScanner m_scanner;
	GameUpdatePacket* m_game_packet;
	uint8_t* m_extended_data;
};