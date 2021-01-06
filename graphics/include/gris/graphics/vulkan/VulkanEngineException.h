#pragma once

#include <gris/graphics/EngineException.h>

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

class VulkanEngineException : public EngineException {
public:
    explicit VulkanEngineException(const std::string& message) : EngineException(message) {}

    VulkanEngineException(const std::string& message, const std::string& details) : EngineException(message, details) {}

    VulkanEngineException(const std::string& message, vk::Result result) : EngineException(message, vk::to_string(result)) {}

    template<typename U>
    VulkanEngineException(const std::string& message, const vk::ResultValue<U>& resultValue) : VulkanEngineException(message, resultValue.result) {}
};
