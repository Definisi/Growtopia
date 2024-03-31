#pragma once
#include <cstdint>
#include <utils/vector.hpp>


struct FloatingItem {
    uint16_t m_item_id;
    vector2_t m_pos;
    uint8_t m_amount;
    uint8_t m_flags;
    uint32_t m_drop_id_offset;

    FloatingItem() : m_item_id(0), m_pos(), m_amount(0), m_flags(0), m_drop_id_offset(0) {}
    ~FloatingItem() = default;

    bool operator==(const FloatingItem& other) const
    {
        return m_drop_id_offset == other.m_drop_id_offset;
    }

};