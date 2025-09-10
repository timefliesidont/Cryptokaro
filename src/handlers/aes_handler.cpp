#include "handlers/aes_handler.hpp"
#include <stdexcept>

nlohmann::json handle_aes_encryption(const nlohmann::json& request) {
    // Encryption logic will go here later.
    throw std::runtime_error("AES encryption not yet implemented.");
}

nlohmann::json handle_aes_decryption(const nlohmann::json& request) {
    // Decryption logic will go here later.
    throw std::runtime_error("AES decryption not yet implemented.");
}
