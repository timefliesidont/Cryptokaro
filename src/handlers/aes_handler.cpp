#include "handlers/aes_handler.hpp"
#include "globals.hpp" // For G_RESOURCE_PATH

#include <iostream>
#include <fstream>

// Crypto++ Headers
#include "cryptopp/osrng.h"  // For AutoSeededRandomPool
#include "cryptopp/aes.h"     // For AES
#include "cryptopp/modes.h"   // For CBC_Mode
#include "cryptopp/filters.h" // For StringSource, StreamTransformationFilter, etc.
#include "cryptopp/hex.h"     // For HexEncoder/HexDecoder
#include "cryptopp/base64.h"  // For Base64Encoder/Base64Decoder
#include "cryptopp/secblock.h"// For SecByteBlock

nlohmann::json perform_aes_encryption(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        // 1. Validate and retrieve inputs from JSON
        if (!payload.contains("key") || !payload.contains("textInput")) {
            throw std::runtime_error("Missing 'key' or 'textInput' in payload.");
        }
        std::string hex_key = payload["key"].get<std::string>();
        std::string plaintext = payload["textInput"].get<std::string>();

        // 2. Decode the hex key into a byte block
        CryptoPP::SecByteBlock key(CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::StringSource ss_key(hex_key, true, new CryptoPP::HexDecoder(new CryptoPP::ArraySink(key, key.size())));

        // 3. Generate a new, cryptographically secure random IV for each encryption
        CryptoPP::AutoSeededRandomPool prng;
        CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
        prng.GenerateBlock(iv, iv.size());

        // 4. Perform AES-256 CBC Encryption
        std::string ciphertext_b64;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource ss_plain(plaintext, true, 
            new CryptoPP::StreamTransformationFilter(e,
                new CryptoPP::Base64Encoder(
                    new CryptoPP::StringSink(ciphertext_b64),
                    false // Do not insert line breaks
                )
            )
        );

        // 5. Hex-encode the IV to send back to the frontend
        std::string iv_hex;
        CryptoPP::StringSource ss_iv(iv, iv.size(), true,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(iv_hex),
                false // Lowercase hex
            )
        );

        // 6. Package the successful response
        response["status"] = "success";
        response["ciphertext"] = ciphertext_b64;
        response["iv"] = iv_hex;

    } catch (const std::exception& e) {
        std::cerr << "[AES ENCRYPT ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response;
}

nlohmann::json perform_aes_decryption(const nlohmann::json& payload) {
    nlohmann::json response;
    try {
        // 1. Validate and retrieve inputs from JSON
        if (!payload.contains("key") || !payload.contains("iv") || !payload.contains("ciphertext")) {
            throw std::runtime_error("Missing 'key', 'iv', or 'ciphertext' in payload.");
        }
        std::string hex_key = payload["key"].get<std::string>();
        std::string hex_iv = payload["iv"].get<std::string>();
        std::string ciphertext_b64 = payload["ciphertext"].get<std::string>();

        // 2. Decode the hex key and hex IV into byte blocks
        CryptoPP::SecByteBlock key(CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::StringSource ss_key(hex_key, true, new CryptoPP::HexDecoder(new CryptoPP::ArraySink(key, key.size())));

        CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
        CryptoPP::StringSource ss_iv(hex_iv, true, new CryptoPP::HexDecoder(new CryptoPP::ArraySink(iv, iv.size())));

        // 3. Perform AES-256 CBC Decryption
        std::string recovered_text;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);
        
        // The StringSource pipeline will decode Base64 and then decrypt
        CryptoPP::StringSource ss_cipher(ciphertext_b64, true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StreamTransformationFilter(d,
                    new CryptoPP::StringSink(recovered_text)
                )
            )
        );

        // 4. Package the successful response
        response["status"] = "success";
        response["plaintext"] = recovered_text;

    } catch (const std::exception& e) {
        // Decryption can fail for many reasons (bad key, bad IV, corrupted data)
        // This catch block is very important.
        std::cerr << "[AES DECRYPT ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = "Decryption failed. Check key, IV, and ciphertext integrity.";
    }
    return response;
}

