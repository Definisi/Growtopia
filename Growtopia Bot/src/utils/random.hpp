#pragma once
#include <random>
#include <chrono>

namespace GrowtopiaBot::Utils {
    extern std::mt19937 rng;
    void seed_random();
    int random(int min, int max) noexcept;
}