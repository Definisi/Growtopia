#include <world/world.hpp>

Tile* World::get_tile(const uint32_t& x, const uint32_t& y) {
    if (x >= m_width || y >= m_height)
        return nullptr;

    return &m_tiles[x + y * m_width];
}
