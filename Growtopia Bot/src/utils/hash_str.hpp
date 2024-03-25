#pragma once
#include <string>
#include <vector>
#include <wtypes.h>

int16_t hash_str(std::string str)
{
    UINT buf = 0x55555555;
    for (BYTE& n : std::vector<BYTE>(str.begin(), str.end()))
        buf = (buf >> 27) + (buf << 5) + n;
    return buf;
}