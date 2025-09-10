#pragma once
#include "nlohmann/json.hpp"

// Performs AES encryption.
// The request object must contain the data to encrypt and the key.
nlohmann::json handle_aes_encryption(const nlohmann::json& request);

// Performs AES decryption.
// The request object must contain the data to decrypt and the key.
nlohmann::json handle_aes_decryption(const nlohmann::json& request);
