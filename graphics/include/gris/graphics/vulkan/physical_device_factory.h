#pragma once

#include <gris/graphics/vulkan/utils.h>

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class PhysicalDevice;
class WindowMixin;

class PhysicalDeviceFactory
{
public:
    [[nodiscard]] static PhysicalDevice FindPhysicalDevice(const WindowMixin & window);

private:
    [[nodiscard]] static std::tuple<bool, DeviceQueueFamilyIndices> IsDeviceSuitable(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface);

    [[nodiscard]] static DeviceQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface);

    [[nodiscard]] static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice & device);

    [[nodiscard]] static vk::SampleCountFlagBits GetMaxUsableSampleCount(const vk::PhysicalDevice & physicalDevice);
};

}  // namespace Gris::Graphics::Vulkan
