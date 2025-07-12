#pragma once
#include <string>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <proton/CRandom.h>


inline int random(int range) {
    if (range == 0) {
        return 0;
    }
    return std::rand() % range;
}
    
inline void get_date_and_time(int* nowmonth, int* nowday, int* nowyear, int* nowhour, int* nowmin, int* nowsec) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    *nowyear = localTime->tm_year + 1900;
    *nowmonth = localTime->tm_mon + 1;
    *nowday = localTime->tm_mday;
    *nowhour = localTime->tm_hour;
    *nowmin = localTime->tm_min;
    *nowsec = localTime->tm_sec;
}


inline void dec_to_hex_string(uint32_t value, uint8_t* pOut, int16_t charArrayMaxSize) {
    static uint8_t digit;
    static int i;
        
    for (i = charArrayMaxSize - 1; i >= 0; i--) {
        digit = uint8_t((value & 0x0f) + 0x30);
        if (digit > 0x39)
            digit += 0x07;
        pOut[i] = digit;
        value >>= 4;
    }
}
    

inline std::string generate_rid() {
    int nowyear, nowmonth, nowday, nowhour, nowmin, nowsec;
    get_date_and_time(&nowmonth, &nowday, &nowyear, &nowhour, &nowmin, &nowsec);
    uint32_t rid[4] = { 0, 0, 0, 0 };
    CRandom r;
    rid[0] = ((nowmonth + ((nowyear - 2014) * 12)) * (259200)) + (nowday * 86400) + (nowhour * 3600) + nowsec;
    rid[1] = (uint32_t)random(RT_RAND_MAX) * (uint32_t)random(RT_RAND_MAX) + (uint32_t)random(RT_RAND_MAX);
    r.SetRandomSeed(rid[0] + random(RT_RAND_MAX) + nowyear);
    rid[2] = r.Random(200000000);
    rid[3] = (uint32_t)random(RT_RAND_MAX) * (uint32_t)random(RT_RAND_MAX) + (uint32_t)random(RT_RAND_MAX);
    uint8_t* rid_data = reinterpret_cast<uint8_t*>(&rid);
    char temp[32];
    std::string final;
    //convert to string
    for (int i = 0; i < 4; i++) {
        memset(temp, 0, 32);
        dec_to_hex_string(rid[i], (BYTE*)temp, 8);
        final += std::string(temp);
    }
    std::transform(final.begin(), final.end(), final.begin(),
        ::toupper);
    return final;
}