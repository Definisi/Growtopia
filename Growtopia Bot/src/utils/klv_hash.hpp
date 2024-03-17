#pragma once
#include <cstdint>
#include <string>

namespace utils {
    std::string klv_hash(const float& game_version, const uint16_t& protocol, const std::string rid);
}