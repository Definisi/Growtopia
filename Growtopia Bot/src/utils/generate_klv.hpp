#pragma once
#include <string>
#include <array>
#include <format>

#include <utils/md5_sha256.hpp>

std::string generate_klv(std::string game_version, std::uint32_t protocol, const std::string& rid)
{
    std::string game_version_formatted = game_version;
    std::array salts = {
        "e9fc40ec08f9ea6393f59c65e37f750aacddf68490c4f92d0d2523a5bc02ea63",
        "c85df9056ee603b849a93e1ebab5dd5f66e1fb8b2f4a8caef8d13b9f9e013fa4",
        "3ca373dffbf463bb337e0fd768a2f395b8e417475438916506c721551f32038d",
        "73eff5914c61a20a71ada81a6fc7780700fb1c0285659b4899bc172a24c14fc1"
    }; 


    return sha256_converter(
        sha256_converter(get_md5_check_sum_as_string(sha256_converter(std::to_string(protocol)))) +
        salts[0] +
        sha256_converter(sha256_converter((game_version_formatted))) +
        salts[1] +
        sha256_converter(get_md5_check_sum_as_string(sha256_converter(rid))) +
        salts[2] +
        sha256_converter(sha256_converter(std::to_string(protocol)) + salts[3])
    );
}