#pragma once

#include <httplib.h>  // Error

#include <cstdint>
#include <exception>
#include <nlohmann/json.hpp>  // json, parse_error
#include <string>
#include <utility>  // move

namespace databento {
// Base class for all databento client library exceptions.
class Exception : public std::exception {
 public:
  explicit Exception(std::string message) : message_{std::move(message)} {}

  const char* what() const noexcept override { return message_.c_str(); }
  const std::string& message() const { return message_; }

 private:
  const std::string message_;
};

class HttpRequestError : public Exception {
 public:
  HttpRequestError(std::string request_path, httplib::Error error_code)
      : Exception{BuildMessage(request_path, error_code)},
        request_path_{std::move(request_path)},
        error_code_{error_code} {}

  const std::string& request_path() const { return request_path_; }
  httplib::Error error_code() const { return error_code_; }

 private:
  static std::string BuildMessage(const std::string& request_path,
                                  httplib::Error error_code);

  const std::string request_path_;
  const httplib::Error error_code_;
};

// Exception indicating a 4XX or 5XX HTTP status code was received from the
// server.
class HttpResponseError : public Exception {
 public:
  HttpResponseError(std::string request_path, std::int32_t status_code,
                    std::string response_body)
      : Exception{BuildMessage(request_path, status_code, response_body)},
        request_path_{std::move(request_path)},
        status_code_{status_code},
        response_body_{std::move(response_body)} {}

  const std::string& request_path() const { return request_path_; }
  std::int32_t status_code() const { return status_code_; }
  const std::string& response_body() const { return response_body_; }

 private:
  static std::string BuildMessage(const std::string& request_path,
                                  std::int32_t status_code,
                                  const std::string& response_body);

  const std::string request_path_;
  // int32 is the representation used by httplib
  const std::int32_t status_code_;
  const std::string response_body_;
};

// Exception indicating an argument to a callable is invalid.
class InvalidArgumentError : public Exception {
 public:
  InvalidArgumentError(std::string method_name, std::string param_name,
                       std::string details)
      : Exception{BuildMessage(method_name, param_name, details)},
        method_name_{std::move(method_name)},
        param_name_{std::move(param_name)},
        details_{std::move(details)} {}

  const std::string& method_name() const { return method_name_; }
  const std::string& argument_name() const { return param_name_; }
  const std::string& details() const { return details_; }

 private:
  static std::string BuildMessage(const std::string& method_name,
                                  const std::string& param_name,
                                  const std::string& details);

  const std::string method_name_;
  const std::string param_name_;
  const std::string details_;
};

// Exception indicating an error parsing a JSON response from the Databento API.
class JsonResponseError : public Exception {
 public:
  static JsonResponseError ParseError(
      const std::string& path,
      const nlohmann::detail::parse_error& parse_error);
  static JsonResponseError MissingKey(const std::string& method_name,
                                      const nlohmann::json& key);
  static JsonResponseError TypeMismatch(const std::string& method_name,
                                        const std::string& expected_type_name,
                                        const nlohmann::json& json);
  static JsonResponseError TypeMismatch(const std::string& method_name,
                                        const std::string& expected_type_name,
                                        const nlohmann::json& key,
                                        const nlohmann::json& value);

 private:
  explicit JsonResponseError(std::string message)
      : Exception{std::move(message)} {}
};

// Exception indicating an error parsing a DBZ response from the Databento API.
class DbzResponseError : public Exception {
 public:
  explicit DbzResponseError(std::string message)
      : Exception{std::move(message)} {}
};
}  // namespace databento
