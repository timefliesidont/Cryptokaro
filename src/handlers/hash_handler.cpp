#include "handlers/hash_handler.hpp"
#include "globals.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector> // Required for std::vector

// Crypto++ headers
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"
#include "cryptopp/filters.h"
#include "cryptopp/hmac.h"
#include "cryptopp/secblock.h" // Still useful for the 'byte' type

nlohmann::json perform_sha256_digest(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        if (!payload.contains("textInput")) {
            throw std::runtime_error("Missing 'textInput' in payload.");
        }
        
        std::string input = payload["textInput"].get<std::string>();
        std::string digest;
        
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource s(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(digest),
                    false // Lowercase hex
                )
            )
        );

        response["status"] = "success";
        response["digest"] = digest;
        
    } catch (const std::exception& e) {
        std::cerr << "[HASH HANDLER ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response;
}

nlohmann::json perform_hmac_digest(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        if (!payload.contains("key") || !payload.contains("textInput")) {
            throw std::runtime_error("Missing 'key' or 'textInput' in payload.");
        }
        std::string hex_key = payload["key"].get<std::string>();
        std::string input = payload["textInput"].get<std::string>();

        // CORRECTION 1: Use std::vector<byte> instead of SecByteBlock to match VectorSink
        std::vector<CryptoPP::byte> key;
        CryptoPP::StringSource ss_key(hex_key, true, new CryptoPP::HexDecoder(new CryptoPP::VectorSink(key)));

        std::string digest;
        // CORRECTION 2: Use key.data() to get a pointer for the HMAC constructor
        CryptoPP::HMAC<CryptoPP::SHA256> hmac(key.data(), key.size());
        CryptoPP::StringSource ss_input(input, true,
            new CryptoPP::HashFilter(hmac,
                new CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(digest),
                    false
                )
            )
        );

        response["status"] = "success";
        response["digest"] = digest;

    } catch (const std::exception& e) {
        std::cerr << "[HMAC HANDLER ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response;
}


nlohmann::json run_integration_test(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        std::string config_path = G_RESOURCE_PATH + "config.json";
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            throw std::runtime_error("Could not find or open config.json.");
        }
        
        nlohmann::json config_data;
        config_file >> config_data;
        std::string message = config_data.at("testMessage").get<std::string>();

        CryptoPP::SHA256 hash;
        std::string digest;
        CryptoPP::StringSource s(message, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(digest),
                    false
                )
            )
        );

        std::stringstream ss;
        ss << "<strong>✅ Integration Test Passed!</strong><br><br>";
        ss << "<strong>Message from config.json:</strong><br>" << message << "<br><br>";
        ss << "<strong>SHA-256 Digest:</strong><br>" << digest;

        response["status"] = "success";
        response["html"] = ss.str();

    } catch (const std::exception& e) {
        std::cerr << "[INTEGRATION TEST ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response;
}

