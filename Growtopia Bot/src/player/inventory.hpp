#pragma once
#include <cstdint>
#include <map>

struct Inventory {
    std::map<uint16_t, uint8_t> m_items;
    uint32_t m_size;
    
    bool has_item(uint16_t id) const;
    std::pair<uint16_t, uint8_t> get_item(uint16_t id) const;
};