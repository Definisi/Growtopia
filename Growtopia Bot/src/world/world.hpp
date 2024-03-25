#pragma once
#include <vector>

#include <world/structs/tile.hpp>
#include <world/structs/floating_item.hpp>

struct World {
	std::string m_name;
	uint16_t m_version;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_tile_count;

	uint32_t m_floating_item_count;
	uint32_t m_last_floating_item_offset;

	std::vector<Tile> m_tiles;
	std::vector<FloatingItem> m_floating_items;

	uint32_t m_owner_uid;
	std::vector<uint32_t> m_access_list;

	Tile* get_tile(const uint32_t& x, const uint32_t& y);
};