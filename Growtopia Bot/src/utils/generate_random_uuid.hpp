#pragma once
#include <iomanip>
#include <random>

#include <utils/generate_random_hex.hpp>

std::string generate_random_uuid() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 15);

	std::stringstream stream;
	for (int index = 0; index < 32; ++index) {
		if (index == 8 || index == 12 || index == 16 || index == 20) {
			stream << "-";
		}
		stream << generate_random_hex(1);
	}

	return stream.str();
}