#pragma once
#include <cstdint>

namespace hash {
	int32_t proton(const char* data, size_t length = 0) {
	    int32_t hash{ 0x55555555 };
	    
	    if (data) {
	        if (length > 0) {
	            while (length--) {
	                hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
	            }
	            return hash;
	        }
	        
	        while (*data) {
	            hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
	        }
	    }
	    
	    return hash;
	}
}