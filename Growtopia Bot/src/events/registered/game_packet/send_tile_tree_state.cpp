#include <events/registered/game_packet/send_tile_tree_state.hpp>

#include <format>
#include <iostream>

#include <item/item_core.hpp>
#include <item/types.hpp>
#include <utils/binary_reader.hpp>
#include <utils/get_current_time.hpp>

namespace events {
	void send_tile_tree_state(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		Tile* tile = ctx.m_client->m_world.get_tile(ctx.m_game_packet->m_tile_pos_x, ctx.m_game_packet->m_tile_pos_y);

		if (tile) {
			if (ctx.m_game_packet->m_item == -1) {
				uint16_t background = tile->m_background;
				uint16_t lock_parent = tile->m_lock_parent;

				tile->m_foreground = 0;
				tile->m_ready_time = 0;
				tile->m_background = background;
				tile->m_lock_parent = lock_parent;
			}
		}
	}
}