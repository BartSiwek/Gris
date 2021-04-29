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
        : runtime_error(message + (details.empty() ? "" : " [" + details + "]"))
    {
    }

    EngineException(const std::string & message, const std::string & details, const std::string & moreDetails)
        : runtime_error(message + (details.empty() ? "" : " [" + details + "]") + (moreDetails.empty() ? "" : " [" + moreDetails + "]"))
    {
    }
};

}  // namespace Gris
