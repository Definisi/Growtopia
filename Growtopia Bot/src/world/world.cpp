#include <world/world.hpp>

Tile* World::get_tile(const uint32_t& x, const uint32_t& y) {
    for (auto& tile : m_tiles) {
        if (tile.m_pos.m_x == x && tile.m_pos.m_y == y)
            return &tile;
    }
    return nullptr;
}