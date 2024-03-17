#pragma once

#include <string>

namespace auth {
	namespace data {
		inline std::string username;
		inline std::string ip;
		inline std::string hwid;

		//response
		inline std::string message;
		inline bool success{};
	};
};
