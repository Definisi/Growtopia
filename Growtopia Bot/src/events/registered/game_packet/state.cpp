#include <events/registered/game_packet/state.hpp>
#include <algorithm>


void events::state(EventContext& ctx)
{
    int net_id = ctx.m_game_packet->m_net_id;
    auto it = std::find_if(ctx.m_client->m_world.m_players.begin(), ctx.m_client->m_world.m_players.end(),
        [net_id](const Player& player) {
            return (int)player.m_net_id == net_id;
        }
    );
    if (it != ctx.m_client->m_world.m_players.end()) {
        it->m_pos.m_x = (int)ctx.m_game_packet->m_pos_x;
        it->m_pos.m_y = (int)ctx.m_game_packet->m_pos_y;
    }
}
