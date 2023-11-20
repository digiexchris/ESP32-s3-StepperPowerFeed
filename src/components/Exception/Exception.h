#pragma once
#include <exception>
#include <string>

class GenericException : std::exception {
public:
    std::string msg;
    GenericException(const char* aMsg);

// #ifdef ESP_PLATFORM
//     virtual const char*
//     what() const override {
// #else
//         _NODISCARD char const* what() const override {
// #endif
//         return msg.c_str();
//     };
};

class NotImplementedException : GenericException {
    public:
        using GenericException::GenericException;
} ;