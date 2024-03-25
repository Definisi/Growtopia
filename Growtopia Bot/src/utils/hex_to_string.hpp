#pragma once
#include <string>
#include <array>

std::string hex_to_string(std::string hex_string)
{
    if (hex_string.size() > 16)
    {
#ifdef _DEBUG
        LogMsg("This isn't hex data, it's probably already stringified, returning as such");
        return hex_string;
#endif
    }
    if (hex_string.empty()) return "";

    const unsigned char* pin = (const unsigned char*)hex_string.c_str();
    const char* hex = "0123456789ABCDEF";
    char temp[64];
    char* pout = temp;
    std::uint32_t i = 0;
    for (; i < hex_string.size() - 1; ++i) {
        *pout++ = hex[(*pin >> 4) & 0xF];
        *pout++ = hex[(*pin++) & 0xF];
    }
    *pout++ = hex[(*pin >> 4) & 0xF];
    *pout++ = hex[(*pin) & 0xF];
    *pout = 0;

    return std::string(temp);
}