#pragma once

#include <string>
#include <exception>

/// Default Patlib namespace.
namespace ptl {

/// Patlib's exception class.
class exception : public std::exception {
    std::string message;

public:
    /// Constructor.
    /// @param message Message describing encountered error.
    exception(const std::string& message) : message(message) {}

    /// Returns explanatory string.
    /// @return Pointer to a null-terminated string with explanatory information.
    const char* what() const override {
        return message.c_str();
    }
};

} // namespace ptl
