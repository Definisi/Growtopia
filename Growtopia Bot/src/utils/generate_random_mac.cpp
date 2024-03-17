#include <utils/generate_random_mac.hpp>

#include <utils/generate_random_hex.hpp>

namespace utils {
    std::string generate_random_mac() {
        std::string mac;
        for (int i = 0; i < 6; ++i) {
            mac += generate_random_hex(2);
            if (i < 5)
                mac += ":";
        }
        return mac;
    }
}