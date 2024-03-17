#include <utils/sha256.hpp>

#include <algorithm>
#include <iostream>

#include <openssl/evp.h>
#include <openssl/sha.h>

namespace utils {
	std::string sha256(const std::string& input) {
		EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
		if (mdctx == nullptr) {
			std::cerr << "Error creating context." << std::endl;
			return "";
		}

		if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
			std::cerr << "Error initializing digest." << std::endl;
			EVP_MD_CTX_free(mdctx);
			return "";
		}

		if (EVP_DigestUpdate(mdctx, input.c_str(), input.length()) != 1) {
			std::cerr << "Error updating digest." << std::endl;
			EVP_MD_CTX_free(mdctx);
			return "";
		}

		unsigned char hash[SHA256_DIGEST_LENGTH];
		unsigned int hash_len;
		if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
			std::cerr << "Error finalizing digest." << std::endl;
			EVP_MD_CTX_free(mdctx);
			return "";
		}

		EVP_MD_CTX_free(mdctx);

		char md5string[65];
		for (int i = 0; i < 32; i++) {
			sprintf(&md5string[i * 2], "%02x", (unsigned int)hash[i]);
		}
		md5string[64] = '\0';

		std::string _hash = std::string(md5string);
		std::transform(_hash.begin(), _hash.end(), _hash.begin(), ::tolower);
		return _hash;
	}
}