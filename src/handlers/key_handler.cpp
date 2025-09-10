#include "handlers/key_handler.hpp"
#include "globals.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>

// Crypto++ headers
#include "cryptopp/osrng.h" // For AutoSeededRandomPool
#include "cryptopp/hex.h"    // For HexEncoder
#include "cryptopp/filters.h"
#include "cryptopp/secblock.h" // For SecByteBlock

nlohmann::json generate_symmetric_key(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        // 1. Read key size from config file
        std::string config_path = G_RESOURCE_PATH + "config.json";
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            throw std::runtime_error("Could not open config.json to read key size.");
        }
        nlohmann::json config_data;
        config_file >> config_data;
        int key_size = config_data.at("aesDefaultKeySize").get<int>();

        // 2. Generate secure random bytes for the key
        CryptoPP::AutoSeededRandomPool prng;
        CryptoPP::SecByteBlock key(key_size);
        prng.GenerateBlock(key, key.size());

        // 3. Hex-encode the key to make it readable
        std::string hex_key;
        CryptoPP::StringSource ss(key, key.size(), true,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(hex_key),
                false // false for lowercase
            )
        );

        // 4. Prepare the successful JSON response
        response["status"] = "success";
        response["key"] = hex_key;

    } catch (const std::exception& e) {
        std::cerr << "[KEY HANDLER ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response;
}

