#include "exception.h"

SDLException::SDLException()
    : std::runtime_error("") {}

SDLException::SDLException(const std::string& msg)
    : std::runtime_error(msg) {}

SDLException::SDLException(const char* msg)
    : std::runtime_error(msg) {}

SDLException::~SDLException() noexcept = default;
