#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <optional>

namespace Gris::Graphics::Vulkan
{

struct DeviceQueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

[[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice & physicalDevice,
                                                            const vk::SurfaceKHR & surface);

}  // namespace Gris::Graphics::Vulkan
