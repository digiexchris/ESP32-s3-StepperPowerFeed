#pragma once
#include <exception>
#include <string>

class GenericException : std::exception {
public:
    std::string msg;
    GenericException(const char* aMsg);
    
#ifdef IDF_PATH
    virtual const char*
    what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override {
#else
        _NODISCARD char const* what() const override {
#endif
        return msg.c_str();
    };
};

class NotImplementedException : GenericException {
    public:
        using GenericException::GenericException;
} ;