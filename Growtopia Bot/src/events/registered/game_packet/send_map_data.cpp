#include <events/registered/game_packet/send_map_data.hpp>

#include <cstdint>
#include <format>
#include <iostream>
#include <string>

#include <proton/vector.hpp>

#include <item/item_core.hpp>
#include <utils/binary_reader.hpp>
#include <world/structs/tile.hpp>
#include <world/structs/floating_item.hpp>

namespace events {
	void send_map_data(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		ctx.m_client->m_world.m_tiles.clear();
		ctx.m_client->m_world.m_floating_items.clear();

		BinaryReader reader(ctx.m_extended_data, ctx.m_game_packet->m_data_size);
		
		ctx.m_client->m_world.m_version = reader.read<uint16_t>();
		reader.skip(4);

		uint16_t length = reader.read<uint16_t>();
		ctx.m_client->m_world.m_name = reader.read(length);
		ctx.m_client->m_world.m_width = reader.read<uint32_t>();
		ctx.m_client->m_world.m_height = reader.read<uint32_t>();
		ctx.m_client->m_world.m_tile_count = reader.read<uint32_t>();
		reader.skip(5);
		for (uint32_t y = 0; y < ctx.m_client->m_world.m_height; ++y) {
			for (uint32_t x = 0; x < ctx.m_client->m_world.m_width; ++x) {
				Tile tile;
				Vector2i pos(x, y);
				tile.m_pos = pos;
				tile.m_foreground = reader.read<uint16_t>();
				tile.m_background = reader.read<uint16_t>();
				tile.m_lock_parent = reader.read<uint16_t>();
				tile.m_flags = reader.read<uint16_t>();

				const Item& item = item_database->get_item(tile.m_foreground);

				if ((tile.m_flags & TileFlag::TILE_EXTRA) || item.m_has_extra)
					tile.read_tile_extra(reader, ctx.m_client->m_world.m_version);

				if (tile.m_foreground == 242 ||
					tile.m_foreground == 1796 ||
					tile.m_foreground == 4802 ||
					tile.m_foreground == 5260 ||
					tile.m_foreground == 7188 ||
					tile.m_foreground == 11550) {
					ctx.m_client->m_world.m_owner_uid = tile.m_owner_uid;
					ctx.m_client->m_world.m_access_list = tile.m_access_list;
				}

				ctx.m_client->m_world.m_tiles.push_back(tile);
			}
		}
		reader.skip(12);
		ctx.m_client->m_world.m_floating_item_count = reader.read<uint32_t>();
		ctx.m_client->m_world.m_last_floating_item_offset = reader.read<uint32_t>();
		for (uint32_t index = 0; index < ctx.m_client->m_world.m_floating_item_count; index++) {
			FloatingItem m_floating_item;
			m_floating_item.item_id = reader.read<uint16_t>();
			m_floating_item.pos.m_x = reader.read<float>();
			m_floating_item.pos.m_y = reader.read<float>();
			m_floating_item.amount  = reader.read<uint8_t>();
			m_floating_item.flags = reader.read<uint8_t>();
			m_floating_item.drop_id_offset = reader.read<uint32_t>();
			ctx.m_client->m_world.m_floating_items.push_back(m_floating_item);
		}
	}
}
