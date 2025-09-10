#ifndef AES_HANDLER_HPP
#define AES_HANDLER_HPP

#include "nlohmann/json.hpp"

// Declares the function that will handle AES encryption requests
nlohmann::json perform_aes_encryption(const nlohmann::json& payload);

// Declares the function that will handle AES decryption requests
nlohmann::json perform_aes_decryption(const nlohmann::json& payload);

#endif // AES_HANDLER_HPP
