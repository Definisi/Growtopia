#include <events/registered/game_packet/ping_request.hpp>

#include <cstdint>

#include <proton/packet.hpp>

namespace events {
	void ping_request(EventContext& ctx) {
		GameUpdatePacket game_packet;
		game_packet.m_type = NET_GAME_PACKET_PING_REPLY;
		ctx.m_client->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));
	}
}