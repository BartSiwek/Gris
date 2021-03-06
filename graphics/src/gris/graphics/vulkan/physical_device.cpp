#include <gris/graphics/vulkan/physical_device.h>

#include "validation_layers.h"

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/utils.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>
#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/assert.h>
#include <gris/utils.h>

#include <set>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PhysicalDevice::PhysicalDevice() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PhysicalDevice::PhysicalDevice(vk::PhysicalDevice physicalDevice, vk::SampleCountFlagBits msaaSamples, DeviceQueueFamilyIndices queueFamilies)
    : m_physicalDevice(physicalDevice)
    , m_msaaSamples(msaaSamples)
    , m_queueFamilies(queueFamilies)
{
    GRIS_ALWAYS_ASSERT(m_physicalDevice, "Physical device must be valid");
    GRIS_ALWAYS_ASSERT(m_queueFamilies.IsComplete(), "Queue family indices must be complete");
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PhysicalDevice::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::PhysicalDevice::IsValid() const
{
    return static_cast<bool>(m_physicalDevice) && m_queueFamilies.IsComplete();
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
        const auto props = m_physicalDevice.getFormatProperties(format, Instance::Dispatch());
        if ((tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            || (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features))
        {
            return format;
        }
    }

    throw VulkanEngineException("Failed to find supported format!");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::FormatProperties Gris::Graphics::Vulkan::PhysicalDevice::GetFormatProperties(vk::Format format) const
{
    return m_physicalDevice.getFormatProperties(format, Instance::Dispatch());
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::PhysicalDevice::SwapChainSupport(const WindowMixin & window) const
{
    return QuerySwapChainSupport(m_physicalDevice, window.SurfaceHandle());
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device Gris::Graphics::Vulkan::PhysicalDevice::CreateDevice() const
{
    auto uniqueQueueFamilies = std::set<uint32_t>{
        m_queueFamilies.graphicsFamily.value(),
        m_queueFamilies.presentFamily.value()
    };

    std::array<float, 1> queuePriority = { 1.0F };
    auto queueCreateInfos = Gris::MakeReservedVector<vk::DeviceQueueCreateInfo>(uniqueQueueFamilies.size());
    for (auto queueFamily : uniqueQueueFamilies)
    {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo{}
                                          .setQueueFamilyIndex(queueFamily)
                                          .setQueuePriorities(queuePriority));
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = static_cast<vk::Bool32>(true);

    std::vector<const char *> enabledLayers;
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        enabledLayers.insert(enabledLayers.end(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
    }

    auto const createInfo = vk::DeviceCreateInfo{}
                                .setQueueCreateInfos(queueCreateInfos)
                                .setPEnabledLayerNames(enabledLayers)
                                .setPEnabledExtensionNames(REQUIRED_EXTENSIONS)
                                .setPEnabledFeatures(&deviceFeatures);

    auto createDeviceResult = m_physicalDevice.createDevice(createInfo, nullptr, Instance::Dispatch());
    if (createDeviceResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating logical device", createDeviceResult);
    }

    return createDeviceResult.value;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator Gris::Graphics::Vulkan::PhysicalDevice::CreateAllocator(const vk::Device & device, const vk::DispatchLoaderDynamic dispatch) const
{
    return Instance::CreateAllocator(m_physicalDevice, device, dispatch);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::PhysicalDevice::Reset()
{
    m_queueFamilies = {};
    m_msaaSamples = vk::SampleCountFlagBits::e1;
    m_physicalDevice = nullptr;
}
