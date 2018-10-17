#pragma once

#include <string>
#include <exception>

namespace ptl {

class exception : public std::exception {
    std::string message;

public:
    exception(const std::string& message) : message(message) {}

    const char* what() const override {
        return message.c_str();
    }
};

}
