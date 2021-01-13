#pragma once

#include <stdexcept>
#include <string>

namespace Gris
{

class EngineException : public std::runtime_error
{
public:
    explicit EngineException(const std::string & message)
        : runtime_error(message)
    {
    }

    EngineException(const std::string & message, const std::string & details)
        : runtime_error(details.empty() ? message : message + " [" + details + "]")
    {
    }
};

}  // namespace Gris
