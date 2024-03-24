#include <events/registered/game_packet/tile_change_request.hpp>

#include <format>
#include <iostream>

#include <item/item_core.hpp>
#include <item/types.hpp>
#include <utils/binary_reader.hpp>
#include <utils/get_current_time.hpp>

namespace events {
	void tile_change_request(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		Tile* tile = ctx.m_client->m_world.get_tile(ctx.m_game_packet->m_tile_pos_x, ctx.m_game_packet->m_tile_pos_y);

		if (tile) {
			if (ctx.m_game_packet->m_item_id == 18) {
				if (tile->m_foreground != 0) {
					uint16_t background = tile->m_background;
					uint16_t lock_parent = tile->m_lock_parent;

					tile->m_foreground = 0;
					tile->m_background = background;
					tile->m_lock_parent = lock_parent;
				}
				else {
					tile->m_background = 0;
				}
			}
			else {
				if (item_database->is_background(ctx.m_game_packet->m_item_id)) {
					tile->m_background = ctx.m_game_packet->m_item_id;
				}
				else {
					tile->m_foreground = ctx.m_game_packet->m_item_id;

					const Item& item = item_database->get_item(tile->m_foreground);
					if (item.m_action_type == ITEM_TYPE_SEED) {
						tile->m_ready_time = get_current_time<std::chrono::seconds>() + item.m_grow_time;
						tile->m_fruit_count = ctx.m_game_packet->m_fruit_count;
					}
				}
			}
		}
	}
}