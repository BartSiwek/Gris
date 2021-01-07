#include <gris/graphics/vulkan/VulkanPhysicalDeviceFactory.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>
#include <gris/graphics/vulkan/VulkanInstance.h>
#include <gris/graphics/vulkan/VulkanPhysicalDevice.h>
#include <gris/graphics/vulkan/VulkanUtils.h>
#include <gris/graphics/vulkan/VulkanWindowMixin.h>

#include <set>

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanPhysicalDevice Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::FindPhysicalDevice(const VulkanWindowMixin & window)
{
    auto devices = VulkanInstance::EnumeratePhysicalDevices();

    for (auto const & device : devices)
    {
        auto [isSuitable, queueFamilies] = IsDeviceSuitable(device, window.SurfaceHandle());
        if (isSuitable)
        {
            auto const msaaSamples = GetMaxUsableSampleCount(device);
            return VulkanPhysicalDevice(device, msaaSamples, queueFamilies);
        }
    }

    throw VulkanEngineException("Failed to find a suitable GPU!");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::tuple<bool, Gris::Graphics::Vulkan::DeviceQueueFamilyIndices> Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::IsDeviceSuitable(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface)
{
    auto queueFamilies = FindQueueFamilies(device, surface);
    auto extensionsSupported = CheckDeviceExtensionSupport(device);

    auto swapChainSupport = SwapChainSupportDetails{};
    auto swapChainAdequate = false;
    if (extensionsSupported)
    {
        swapChainSupport = QuerySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    auto supportedFeatures = device.getFeatures();
    auto isSuitable = queueFamilies.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    return { isSuitable, queueFamilies };
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::DeviceQueueFamilyIndices Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::FindQueueFamilies(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface)
{
    auto queueFamilies = device.getQueueFamilyProperties();

    DeviceQueueFamilyIndices indices;
    auto i = 0;
    for (auto const & queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        auto const surfaceSupportResult = device.getSurfaceSupportKHR(i, surface);
        if (surfaceSupportResult.result != vk::Result::eSuccess)
            throw VulkanEngineException("Error getting surface support for physical device", surfaceSupportResult);

        if (surfaceSupportResult.value)
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

[[nodiscard]] bool Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::CheckDeviceExtensionSupport(const vk::PhysicalDevice & device)
{
    auto availableExtensionsResult = device.enumerateDeviceExtensionProperties();
    if (availableExtensionsResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error enumerating physical device extension properties", availableExtensionsResult);

    std::set<std::string> requiredExtensions(VulkanPhysicalDevice::REQUIRED_EXTENSIONS.begin(), VulkanPhysicalDevice::REQUIRED_EXTENSIONS.end());
    for (auto const & extension : availableExtensionsResult.value)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::SampleCountFlagBits Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::GetMaxUsableSampleCount(const vk::PhysicalDevice & physicalDevice)
{
    auto const physicalDeviceProperties = physicalDevice.getProperties();

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