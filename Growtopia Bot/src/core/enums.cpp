#include "enums.hpp"

/**
 * @file enums.cpp
 * @brief Implementation of enum utility functions
 * @author Growtopia Bot Team
 */

namespace GrowtopiaBot {
    const char* bot_status_to_string(BotStatus status) {
        switch (status) {
            case BotStatus::OFFLINE: return "OFFLINE";
            case BotStatus::ONLINE: return "ONLINE";
            case BotStatus::CONNECTED: return "CONNECTED";
            case BotStatus::DISCONNECTED: return "DISCONNECTED";
            case BotStatus::SUSPENDED: return "SUSPENDED";
            case BotStatus::TEMPBAN: return "TEMPBAN";
            case BotStatus::IPBAN: return "IPBAN";
            case BotStatus::LOGINFAILED: return "LOGINFAILED";
            case BotStatus::GUESTCAPTCHA: return "GUESTCAPTCHA";
            case BotStatus::WRONGPASS: return "WRONGPASS";
            case BotStatus::CHANGESERVER: return "CHANGESERVER";
            case BotStatus::GOTCAPTCHA: return "GOTCAPTCHA";
            case BotStatus::WRONGCAPTCHA: return "WRONGCAPTCHA";
            case BotStatus::ONEXIT: return "ONEXIT";
            case BotStatus::ONWORLD: return "ONWORLD";
            case BotStatus::INVALIDEMAIL: return "INVALIDEMAIL";
            case BotStatus::AAP: return "AAP";
            case BotStatus::MAXIPADDRESS: return "MAXIPADDRESS";
            case BotStatus::ERCON: return "ERCON";
            case BotStatus::FAILEDENTERINGWORLD: return "FAILEDENTERINGWORLD";
            case BotStatus::UPDATE_REQUIRED: return "UPDATE_REQUIRED";
            case BotStatus::FORBIDDEN: return "FORBIDDEN";
            case BotStatus::FORBIDDEN1: return "FORBIDDEN1";
            case BotStatus::FORBIDDEN2: return "FORBIDDEN2";
            default: return "UNKNOWN";
        }
    }
}