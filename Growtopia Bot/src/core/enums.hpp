#pragma once

/**
 * @file enums.hpp
 * @brief Type-safe enumerations for the Growtopia Bot application
 * @author Growtopia Bot Team
 */

namespace GrowtopiaBot {
    /**
     * @enum BotStatus
     * @brief Represents the current status of a bot client
     */
    enum class BotStatus {
        OFFLINE,
        ONLINE,
        CONNECTED,
        DISCONNECTED,
        SUSPENDED,
        TEMPBAN,
        IPBAN,
        LOGINFAILED,
        GUESTCAPTCHA,
        WRONGPASS,
        CHANGESERVER,
        GOTCAPTCHA,
        WRONGCAPTCHA,
        ONEXIT,
        ONWORLD,
        INVALIDEMAIL,
        AAP,
        MAXIPADDRESS,
        ERCON,
        FAILEDENTERINGWORLD,
        UPDATE_REQUIRED,
        FORBIDDEN,
        FORBIDDEN1,
        FORBIDDEN2
    };
    
    /**
     * @brief Convert BotStatus enum to string representation
     * @param status The BotStatus to convert
     * @return String representation of the status
     */
    const char* bot_status_to_string(BotStatus status);
    
    /**
     * @enum LogLevel
     * @brief Logging levels for the application
     */
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    /**
     * @enum NetworkProtocol
     * @brief Network protocol types
     */
    enum class NetworkProtocol {
        HTTP,
        HTTPS,
        TCP,
        UDP
    };
}