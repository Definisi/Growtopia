#include <events/registered/game_packet/ping_request.hpp>

#include <cstdint>

#include <proton/packet.hpp>

namespace events {
	void ping_request(EventContext& ctx) {
		GameUpdatePacket* game_packet_in = ctx.m_game_packet;

		GameUpdatePacket game_packet_out;
		
		game_packet_out.m_type = NET_GAME_PACKET_PING_REPLY;
		game_packet_out.m_ping_hash = game_packet_in->m_ping_hash;
		game_packet_out.m_int_data = game_packet_in->m_int_data;
		game_packet_out.m_pos_x = 64.f;
		game_packet_out.m_pos_y = 64.f;
		game_packet_out.m_velocity_x = 1000.f;
		game_packet_out.m_velocity_y = 250.f;
		ctx.m_client->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet_out, sizeof(GameUpdatePacket));
	}
}