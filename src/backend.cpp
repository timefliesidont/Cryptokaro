#include "backend.hpp"
#include <iostream>
#include "nlohmann/json.hpp"

// Include all our handlers
#include "handlers/hash_handler.hpp"
#include "handlers/aes_handler.hpp"
#include "handlers/key_handler.hpp"

std::string invoke(const std::string& raw_req) {
    nlohmann::json response;
    try {
        // HIGHLIGHT: The webview library wraps the request in an array.
        // We must parse it as an array and take the first element.
        auto json_array = nlohmann::json::parse(raw_req);

        if (!json_array.is_array() || json_array.empty()) {
            throw std::runtime_error("Invalid request format: expected a non-empty array.");
        }

        // The actual request object is the first element of the array.
        const auto& req_json = json_array[0];

        if (!req_json.is_object()) {
             throw std::runtime_error("Invalid request format: first array element must be an object.");
        }

        std::string operation = req_json.at("operation").get<std::string>();
        const auto& payload = req_json.at("payload");

        if (operation == "integrationTest") {
            response = run_integration_test(payload);
        } 
        else if (operation == "sha256") {
            response = perform_sha256_digest(payload);
        }
        else if (operation == "generateKey") {
            response = generate_symmetric_key(payload);
        }
        else if (operation == "hmac") {
            response = perform_hmac_digest(payload);
        }
        // TODO: Add other operations (aes, keygen, hmac) here in the future
        else {
            throw std::runtime_error("Unknown operation: " + operation);
        }

    } catch (const std::exception& e) {
        std::cerr << "[BACKEND ROUTER ERROR] " << e.what() << std::endl;
        response["status"] = "error";
        response["error"] = e.what();
    }
    return response.dump();
}

