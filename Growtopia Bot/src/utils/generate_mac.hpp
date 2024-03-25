#include <iomanip>
#include <sstream>
#include <random>

std::string generate_mac() {
    std::stringstream mac_stream;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0x00, 0xff);

    for (int i = 0; i < 6; ++i) {
        mac_stream << std::setw(2) << std::setfill('0') << std::hex << dis(gen);
        if (i < 5) {
            mac_stream << ":";
        }
    }

    return mac_stream.str();
}