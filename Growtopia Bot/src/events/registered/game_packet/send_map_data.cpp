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
		auto& world = ctx.m_client->m_world;

		world.m_tiles.clear();
		world.m_floating_items.clear();

		BinaryReader reader(ctx.m_extended_data, ctx.m_game_packet->m_data_size);
		
		world.m_version = reader.read<uint16_t>();
		reader.skip(4);

		uint16_t length = reader.read<uint16_t>();
		world.m_name = reader.read(length);
		world.m_width = reader.read<uint32_t>();
		world.m_height = reader.read<uint32_t>();
		world.m_tile_count = reader.read<uint32_t>();
		reader.skip(5);
		uint32_t width = world.m_width;

		for (uint32_t i = 0; i < world.m_height * width; ++i) {
			uint32_t y = i / width;
			uint32_t x = i % width;

			Tile tile;
			Vector2i pos(x, y);
			tile.m_pos = pos;
			tile.m_foreground = reader.read<uint16_t>();
			tile.m_background = reader.read<uint16_t>();
			tile.m_lock_parent = reader.read<uint16_t>();
			tile.m_flags = reader.read<uint16_t>();

			const Item& item = item_database->get_item(tile.m_foreground);

			if (tile.m_lock_parent)
				reader.skip(2);

			if ((tile.m_flags & TileFlag::TILE_EXTRA) || item.m_has_extra)
				tile.read_tile_extra(reader, world.m_version);

			if (tile.m_foreground == 242 ||
				tile.m_foreground == 1796 ||
				tile.m_foreground == 4802 ||
				tile.m_foreground == 5260 ||
				tile.m_foreground == 7188 ||
				tile.m_foreground == 11550) {
				world.m_owner_uid = tile.m_owner_uid;
				world.m_access_list = tile.m_access_list;
			}
			//std::cout << "Index " << i << " | fg " << tile.m_foreground << " | bg " << tile.m_background << std::endl;
			world.m_tiles.push_back(tile);
		}

		reader.skip(12);
		world.m_floating_item_count = reader.read<uint32_t>();
		world.m_last_floating_item_offset = reader.read<uint32_t>();
		for (uint32_t index = 0; index < world.m_floating_item_count; index++) {
			FloatingItem m_floating_item;
			m_floating_item.m_item_id = reader.read<uint16_t>();
			m_floating_item.m_pos.m_x = reader.read<float>();
			m_floating_item.m_pos.m_y = reader.read<float>();
			m_floating_item.m_amount  = reader.read<uint8_t>();
			m_floating_item.m_flags = reader.read<uint8_t>();
			m_floating_item.m_drop_id_offset = reader.read<uint32_t>();
			world.m_floating_items.push_back(m_floating_item);
		}
		ctx.m_client->status = BotStatus::ONLINE;
	}
}
