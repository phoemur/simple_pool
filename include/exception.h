#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

class SDLException : public std::runtime_error
{
public:
    SDLException();
    SDLException(const std::string& msg);
    SDLException(const char* msg);
    virtual ~SDLException() noexcept;
};

#endif // EXCEPTION_H
