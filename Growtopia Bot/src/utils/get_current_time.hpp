#pragma once
#include <chrono>

namespace utils {
    template<typename T>
    inline uint64_t get_current_time() {
        return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}