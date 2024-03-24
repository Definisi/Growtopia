#include <events/registered/game_packet/modify_item_inventory.hpp>

#include <item/item_core.hpp>

namespace events {
	void modify_item_inventory(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		if (!ctx.m_client->m_inventory.has_item(ctx.m_game_packet->m_item_id)) {
			ctx.m_client->m_inventory.m_items[ctx.m_game_packet->m_item_id] = ctx.m_game_packet->m_gained_item_count;
			return;
		}

		for (auto& item : ctx.m_client->m_inventory.m_items) {
			if (item.first == ctx.m_game_packet->m_item_id) {
				if (ctx.m_game_packet->m_lost_item_count > 0 && item.second >= ctx.m_game_packet->m_lost_item_count) {
					item.second -= ctx.m_game_packet->m_lost_item_count;
				}
				else if (ctx.m_game_packet->m_gained_item_count > 0) {
					item.second += ctx.m_game_packet->m_gained_item_count;
				}
				else {
					ctx.m_client->m_inventory.m_items.erase(item.first);
				}

				return;
			}
		}
	}
}