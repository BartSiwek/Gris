#pragma once

#include <gris/graphics/EngineException.h>

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class VulkanEngineException : public EngineException
{
public:
    explicit VulkanEngineException(const std::string & message)
        : EngineException(message)
    {
    }

    VulkanEngineException(const std::string & message, const std::string & details)
        : EngineException(message, details)
    {
    }

    VulkanEngineException(const std::string & message, vk::Result result)
        : EngineException(message, vk::to_string(result))
    {
    }

    template<typename U>
    VulkanEngineException(const std::string & message, const vk::ResultValue<U> & resultValue)
        : VulkanEngineException(message, resultValue.result)
    {
    }
};

}  // namespace Gris::Graphics::Vulkan
