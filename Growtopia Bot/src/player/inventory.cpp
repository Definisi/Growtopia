#include <player/inventory.hpp>

bool Inventory::has_item(uint16_t id) const {
    return m_items.find(id) != m_items.end();
}

std::pair<uint16_t, uint8_t> Inventory::get_item(uint16_t id) const {
    for (const auto& item : m_items) {
        if (item.first == id) {
            return item;
        }
    }
    return std::make_pair<uint16_t, uint8_t>(0, 0);
}