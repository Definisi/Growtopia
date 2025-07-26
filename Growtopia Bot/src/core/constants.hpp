#pragma once

/**
 * @file constants.hpp
 * @brief Global constants for the Growtopia Bot application
 * @author Growtopia Bot Team
 */

namespace GrowtopiaBot {
    // Network Configuration
    constexpr int DEFAULT_HTTP_PORT = 8080;
    constexpr int POLLING_DELAY_MS = 10;
    constexpr int DEFAULT_MOVE_DELAY_MS = 200;
    constexpr int PING_TIMEOUT_MS = 500;
    
    // Discord integration removed
    
    // File Paths
    constexpr const char* ITEMS_DATABASE_PATH = "items.dat";
    
    // Game Constants
    constexpr int RANDOM_RANGE_MAX = 200000000;
    constexpr int BASE_YEAR = 2014;
    constexpr int SECONDS_PER_DAY = 86400;
    constexpr int SECONDS_PER_HOUR = 3600;
    constexpr int MINUTES_PER_HOUR = 60;
    constexpr int MONTHS_PER_YEAR = 12;
    constexpr int MONTH_SECONDS = 259200;
    
    // Hex Conversion
    constexpr int HEX_CHAR_ARRAY_SIZE = 8;
    constexpr int RID_COMPONENTS = 4;
    constexpr int TEMP_BUFFER_SIZE = 32;
    
    // Network Addresses
    constexpr const char* DEFAULT_SOCKS5_IP = "0.0.0.0";
    constexpr const char* LOCALHOST_IP = "localhost";
    
    // API Endpoints
    constexpr const char* API_VERSION = "1.0";
    constexpr const char* API_BASE_PATH = "/api/";
} // namespace GrowtopiaBot
