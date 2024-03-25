#include <events/registered/game_packet/send_tile_update_data_multiple.hpp>

#include <cstdint>

#include <item/item_core.hpp>
#include <utils/binary_reader.hpp>

namespace events {
	void send_tile_update_data_multiple(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		BinaryReader reader(ctx.m_extended_data, ctx.m_game_packet->m_data_size);

		while (reader.get_offset() < reader.get_size() - 5) {
			uint32_t x = reader.read<uint32_t>();
			uint32_t y = reader.read<uint32_t>();

			Tile* tile = ctx.m_client->m_world.get_tile(x, y);
			if (tile) {
				tile->m_foreground = reader.read<uint16_t>();
				tile->m_background = reader.read<uint16_t>();
				tile->m_lock_parent = reader.read<uint16_t>();
				tile->m_flags = reader.read<uint16_t>();

				const Item& item = item_database->get_item(tile->m_foreground);

				if ((tile->m_flags & TileFlag::TILE_EXTRA) || item.m_has_extra)
					tile->read_tile_extra(reader, ctx.m_client->m_world.m_version);

				if (tile->m_foreground == 242 ||
					tile->m_foreground == 1796 ||
					tile->m_foreground == 4802 ||
					tile->m_foreground == 5260 ||
					tile->m_foreground == 7188 ||
					tile->m_foreground == 11550) {
					ctx.m_client->m_world.m_owner_uid = tile->m_owner_uid;
					ctx.m_client->m_world.m_access_list = tile->m_access_list;
				}
			}
		}
	}
}