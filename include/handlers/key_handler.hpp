#ifndef KEY_HANDLER_HPP
#define KEY_HANDLER_HPP

#include "nlohmann/json.hpp"

// Generates a cryptographically secure random key.
nlohmann::json generate_symmetric_key(const nlohmann::json& payload);

#endif // KEY_HANDLER_HPP
