#pragma once

#include "VulkanUtils.h"

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

namespace Gris::Graphics::Vulkan
{

class VulkanAllocator;
class VulkanWindowMixin;

class VulkanPhysicalDevice
{
public:
    constexpr static std::array REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanPhysicalDevice(vk::PhysicalDevice physicalDevice, vk::SampleCountFlagBits msaaSamples, DeviceQueueFamilyIndices queueFamilies);

    [[nodiscard]] const vk::SampleCountFlagBits& MsaaSamples() const;
    [[nodiscard]] const DeviceQueueFamilyIndices& QueueFamilies() const;

    [[nodiscard]] vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& features) const;
    [[nodiscard]] vk::FormatProperties GetFormatProperties(vk::Format format) const;

    [[nodiscard]] SwapChainSupportDetails SwapChainSupport(const VulkanWindowMixin& window) const;
    [[nodiscard]] vk::UniqueDevice CreateDevice() const;
    [[nodiscard]] VulkanAllocator CreateVulkanAllocator(const vk::Device& device) const;

private:
    vk::PhysicalDevice m_physicalDevice = {};
    vk::SampleCountFlagBits m_msaaSamples = vk::SampleCountFlagBits::e1;
    DeviceQueueFamilyIndices m_queueFamilies = {};
};

}  // namespace Gris::Graphics::Vulkan
