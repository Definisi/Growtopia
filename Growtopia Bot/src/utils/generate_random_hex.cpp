#include <utils/generate_random_hex.hpp>

#include <iomanip>
#include <random>
#include <sstream>

namespace utils {
    std::string generate_random_hex(size_t length) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);

		std::ostringstream stream;
		stream << std::hex << std::setfill('0');

		for (size_t index = 0; index < length; ++index) {
			stream << std::setw(1) << dis(gen);
		}

		return stream.str();
	}
}