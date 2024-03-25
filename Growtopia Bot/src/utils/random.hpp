#include <random>
typedef std::mt19937 rng_type;
rng_type rng;
void seed_random()
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed((uint32_t)seed);
}

int random(int min, int max) noexcept
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}