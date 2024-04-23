#include <events/registered/game_packet/set_character_state.hpp>

#include <iostream>

namespace events {
	void set_character_state(EventContext& ctx) {
		if (ctx.m_game_packet->m_net_id == ctx.m_client->m_player.m_net_id) {
			ctx.m_client->m_player.m_character_state = ctx.m_game_packet->m_effect_flags;
		}
		else {
            int net_id = ctx.m_game_packet->m_net_id;
            auto it = std::find_if(ctx.m_client->m_world.m_players.begin(), ctx.m_client->m_world.m_players.end(),
                [net_id](const Player& player) {
                    return player.m_net_id == net_id;
                }
            );

            if (it != ctx.m_client->m_world.m_players.end()) {
                it->m_character_state = ctx.m_game_packet->m_effect_flags;
            }
		}
	}
}