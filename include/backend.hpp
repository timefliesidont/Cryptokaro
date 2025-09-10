#ifndef BACKEND_HPP
#define BACKEND_HPP

#include <string>

/**
 * @brief The single entry point for all frontend to backend communication.
 * * @param request_json_str A string containing a JSON object with an "operation"
 * and a "payload".
 * @return std::string A JSON string representing the result of the operation.
 */
std::string invoke(const std::string& request_json_str);

#endif // BACKEND_HPP
