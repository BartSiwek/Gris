#include <gris/graphics/vulkan/physical_device.h>

#include "validation_layers.h"

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/utils.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>
#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/assert.h>

#include <set>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PhysicalDevice::PhysicalDevice(vk::PhysicalDevice physicalDevice, vk::SampleCountFlagBits msaaSamples, DeviceQueueFamilyIndices queueFamilies)
    : m_physicalDevice(physicalDevice)
    , m_msaaSamples(msaaSamples)
    , m_queueFamilies(queueFamilies)
{
    GRIS_ALAWYS_ASSERT(m_physicalDevice, "Physical device must be valid");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::SampleCountFlagBits & Gris::Graphics::Vulkan::PhysicalDevice::MsaaSamples() const
{
    return m_msaaSamples;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::DeviceQueueFamilyIndices & Gris::Graphics::Vulkan::PhysicalDevice::QueueFamilies() const
{
    return m_queueFamilies;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format Gris::Graphics::Vulkan::PhysicalDevice::FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const
{
    for (auto const & format : candidates)
    {
        const auto props = m_physicalDevice.getFormatProperties(format);
        if ((tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            || (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features))
            return format;
    }

    throw VulkanEngineException("Failed to find supported format!");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::FormatProperties Gris::Graphics::Vulkan::PhysicalDevice::GetFormatProperties(vk::Format format) const
{
    return m_physicalDevice.getFormatProperties(format);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::PhysicalDevice::SwapChainSupport(const WindowMixin & window) const
{
    return QuerySwapChainSupport(m_physicalDevice, window.SurfaceHandle());
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::UniqueDevice Gris::Graphics::Vulkan::PhysicalDevice::CreateDevice() const
{
    auto uniqueQueueFamilies = std::set<uint32_t>{
        m_queueFamilies.graphicsFamily.value(),
        m_queueFamilies.presentFamily.value()
    };

    std::array<float, 1> queuePriority = { 1.0f };
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    for (auto queueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, queueFamily, 1, queuePriority.data());
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = true;

    std::vector<const char *> enabledLayers;
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        enabledLayers.insert(enabledLayers.end(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
    }

    vk::DeviceCreateInfo createInfo({}, queueCreateInfos, enabledLayers, REQUIRED_EXTENSIONS, &deviceFeatures);

    auto createDeviceResult = m_physicalDevice.createDeviceUnique(createInfo);
    if (createDeviceResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating logical device", createDeviceResult);

    return std::move(createDeviceResult.value);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator Gris::Graphics::Vulkan::PhysicalDevice::CreateAllocator(const vk::Device & device) const
{
    return Instance::CreateAllocator(m_physicalDevice, device);
}
