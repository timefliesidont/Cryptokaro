#ifndef HASH_HANDLER_HPP
#define HASH_HANDLER_HPP

#include "nlohmann/json.hpp"

// Calculates the SHA-256 digest of a given input.
nlohmann::json perform_sha256_digest(const nlohmann::json& payload);

// Calculates the HMAC-SHA256 digest of a given input using a key.
nlohmann::json perform_hmac_digest(const nlohmann::json& payload);

// Runs the backend integration test.
nlohmann::json run_integration_test(const nlohmann::json& payload);

#endif // HASH_HANDLER_HPP
