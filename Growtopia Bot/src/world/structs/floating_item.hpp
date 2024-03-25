#pragma once
#include <cstdint>
#include <utils/vector.hpp>


struct FloatingItem {
    uint16_t item_id;
    vector2_t pos;
    uint8_t amount;
    uint8_t flags;
    uint32_t drop_id_offset;

    FloatingItem() : item_id(0), pos(), amount(0), flags(0), drop_id_offset(0) {}
    ~FloatingItem() = default;

    bool operator==(const FloatingItem& other) const
    {
        return drop_id_offset == other.drop_id_offset;
    }

};