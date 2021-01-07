#pragma once

#include <gris/graphics/vulkan/VulkanUtils.h>

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

namespace Gris::Graphics::Vulkan
{

class VulkanPhysicalDevice;
class VulkanWindowMixin;

class VulkanPhysicalDeviceFactory
{
public:
    [[nodiscard]] static VulkanPhysicalDevice FindPhysicalDevice(const VulkanWindowMixin& window);

private:
    [[nodiscard]] static std::tuple<bool, DeviceQueueFamilyIndices> IsDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

    [[nodiscard]] static DeviceQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

    [[nodiscard]] static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device);

    [[nodiscard]] static vk::SampleCountFlagBits GetMaxUsableSampleCount(const vk::PhysicalDevice& physicalDevice);
};

}  // namespace Gris::Graphics::Vulkan
