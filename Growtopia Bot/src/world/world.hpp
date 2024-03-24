#pragma once
#include <vector>

#include <world/structs/tile.hpp>

struct World {
	std::string m_name;
	uint16_t m_version;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_tile_count;

	std::vector<Tile> m_tiles;

	uint32_t m_owner_uid;
	std::vector<uint32_t> m_access_list;

	Tile* get_tile(const uint32_t& x, const uint32_t& y);
};