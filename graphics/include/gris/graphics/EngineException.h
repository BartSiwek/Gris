#pragma once

#include <string>
#include <stdexcept>

namespace Gris::Graphics
{

class EngineException : public std::runtime_error
{
public:
    explicit EngineException(const std::string& message) : runtime_error(message)
    {}

    EngineException(const std::string& message, const std::string& details) : runtime_error(details.empty() ? message : message + " [" + details + "]")
    {}
};

}  // namespace Gris::Graphics