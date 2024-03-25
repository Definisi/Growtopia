#pragma once

#include <md5/simple_md5.hpp>
#include <sha256/SHA256.hpp>

#include <utils/hex_to_string.hpp>

std::string sha256_converter(std::string a)
{
    SHA256 sha;
    sha.update(a);
    uint8_t* digest = sha.digest();
    return SHA256::toString(digest);
}
std::string get_md5_check_sum_as_string(std::string inputData)
{

    if (inputData.empty())
        return "";

    md5_state_t state;
    md5_byte_t digest[16];
    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)&inputData[0], (int)inputData.size());
    md5_finish(&state, digest);

    std::string tmp;
    tmp.resize(16);
    memcpy(&tmp[0], digest, 16);

    return hex_to_string(tmp);
}
