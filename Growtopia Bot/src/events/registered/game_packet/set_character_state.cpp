#include <events/registered/game_packet/set_character_state.hpp>

#include <iostream>

namespace events {
	void set_character_state(EventContext& ctx) {
		if (ctx.m_game_packet->m_net_id == ctx.m_client->m_player.m_net_id) {
			ctx.m_client->m_player.m_character_state = ctx.m_game_packet->m_effect_flags;
		}
	}
}