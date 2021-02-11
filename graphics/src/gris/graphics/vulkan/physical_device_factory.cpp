#include <gris/graphics/vulkan/physical_device_factory.h>

#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/physical_device.h>
#include <gris/graphics/vulkan/utils.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>
#include <gris/graphics/vulkan/window_mixin.h>

#include <set>

// -------------------------------------------------------------------------------------------------

namespace
{

[[nodiscard]] Gris::Graphics::Vulkan::DeviceQueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface)
{
    using namespace Gris::Graphics::Vulkan;

    auto queueFamilies = device.getQueueFamilyProperties(Instance::Dispatch());

    DeviceQueueFamilyIndices indices;
    auto i = 0U;
    for (auto const & queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        auto const surfaceSupportResult = device.getSurfaceSupportKHR(i, surface, Instance::Dispatch());
        if (surfaceSupportResult.result != vk::Result::eSuccess)
        {
            throw VulkanEngineException("Error getting surface support for physical device", surfaceSupportResult);
        }

        if (surfaceSupportResult.value != 0U)
        {
            indices.presentFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool CheckDeviceExtensionSupport(const vk::PhysicalDevice & device)
{
    using namespace Gris::Graphics::Vulkan;

    auto availableExtensionsResult = device.enumerateDeviceExtensionProperties(nullptr, Instance::Dispatch());
    if (availableExtensionsResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error enumerating physical device extension properties", availableExtensionsResult);
    }

    std::set<std::string> requiredExtensions(PhysicalDevice::REQUIRED_EXTENSIONS.begin(), PhysicalDevice::REQUIRED_EXTENSIONS.end());
    for (auto const & extension : availableExtensionsResult.value)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::tuple<bool, Gris::Graphics::Vulkan::DeviceQueueFamilyIndices> IsDeviceSuitable(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface)
{
    using namespace Gris::Graphics::Vulkan;

    auto queueFamilies = FindQueueFamilies(device, surface);
    auto extensionsSupported = CheckDeviceExtensionSupport(device);

    auto swapChainSupport = SwapChainSupportDetails{};
    auto swapChainAdequate = false;
    if (extensionsSupported)
    {
        swapChainSupport = QuerySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    auto supportedFeatures = device.getFeatures(Instance::Dispatch());
    auto isSuitable = queueFamilies.IsComplete() && extensionsSupported && swapChainAdequate && static_cast<bool>(supportedFeatures.samplerAnisotropy);
    return { isSuitable, queueFamilies };
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::SampleCountFlagBits GetMaxUsableSampleCount(const vk::PhysicalDevice & physicalDevice)
{
    auto const physicalDeviceProperties = physicalDevice.getProperties(Gris::Graphics::Vulkan::Instance::Dispatch());

    auto const counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64)
    {
        return vk::SampleCountFlagBits::e64;
    }
    if (counts & vk::SampleCountFlagBits::e32)
    {
        return vk::SampleCountFlagBits::e32;
    }
    if (counts & vk::SampleCountFlagBits::e16)
    {
        return vk::SampleCountFlagBits::e16;
    }
    if (counts & vk::SampleCountFlagBits::e8)
    {
        return vk::SampleCountFlagBits::e8;
    }
    if (counts & vk::SampleCountFlagBits::e4)
    {
        return vk::SampleCountFlagBits::e4;
    }
    if (counts & vk::SampleCountFlagBits::e2)
    {
        return vk::SampleCountFlagBits::e2;
    }

    return vk::SampleCountFlagBits::e1;
}

}  // namespace

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::PhysicalDevice Gris::Graphics::Vulkan::FindSuitablePhysicalDevice(const WindowMixin & window)
{
    auto devices = Instance::EnumeratePhysicalDevices();

    for (auto const & device : devices)
    {
        auto [isSuitable, queueFamilies] = IsDeviceSuitable(device, window.SurfaceHandle());
        if (isSuitable)
        {
            auto const msaaSamples = GetMaxUsableSampleCount(device);
            return PhysicalDevice(device, msaaSamples, queueFamilies);
        }
    }

    throw VulkanEngineException("Failed to find a suitable GPU!");
}
