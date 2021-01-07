#include <gris/graphics/vulkan/VulkanDevice.h>

#include <gris/graphics/vulkan/VulkanBuffer.h>
#include <gris/graphics/vulkan/VulkanDeferredContext.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>
#include <gris/graphics/vulkan/VulkanFence.h>
#include <gris/graphics/vulkan/VulkanFramebuffer.h>
#include <gris/graphics/vulkan/VulkanImmediateContext.h>
#include <gris/graphics/vulkan/VulkanPipelineStateObject.h>
#include <gris/graphics/vulkan/VulkanSampler.h>
#include <gris/graphics/vulkan/VulkanSemaphore.h>
#include <gris/graphics/vulkan/VulkanShader.h>
#include <gris/graphics/vulkan/VulkanSwapChain.h>
#include <gris/graphics/vulkan/VulkanTexture.h>
#include <gris/graphics/vulkan/VulkanTextureView.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanDevice::VulkanDevice(VulkanPhysicalDevice physicalDevice)
    : m_physicalDevice(std::move(physicalDevice))
{
    m_device = m_physicalDevice.CreateDevice();
    m_allocator = m_physicalDevice.CreateVulkanAllocator(m_device.get());
    m_context = std::make_unique<VulkanImmediateContext>(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::VulkanImmediateContext * Gris::Graphics::Vulkan::VulkanDevice::Context() const
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanImmediateContext * Gris::Graphics::Vulkan::VulkanDevice::Context()
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::SampleCountFlagBits & Gris::Graphics::Vulkan::VulkanDevice::MsaaSamples() const
{
    return m_physicalDevice.MsaaSamples();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::DeviceQueueFamilyIndices & Gris::Graphics::Vulkan::VulkanDevice::QueueFamilies() const
{
    return m_physicalDevice.QueueFamilies();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::VulkanDevice::SwapChainSupport(const VulkanWindowMixin & window) const
{
    return m_physicalDevice.SwapChainSupport(window);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDevice::WaitIdle()
{
    auto const waitResult = m_device->waitIdle();
    if (waitResult != vk::Result::eSuccess)
        throw VulkanEngineException("Idle wait failed", waitResult);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format Gris::Graphics::Vulkan::VulkanDevice::FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const
{
    return m_physicalDevice.FindSupportedFormat(candidates, tiling, features);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::FormatProperties Gris::Graphics::Vulkan::VulkanDevice::GetFormatProperties(vk::Format format) const
{
    return m_physicalDevice.GetFormatProperties(format);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Device & Gris::Graphics::Vulkan::VulkanDevice::DeviceHandle() const
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Device & Gris::Graphics::Vulkan::VulkanDevice::DeviceHandle()
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDevice::CreateDescriptorPool(uint32_t imageCount)
{
    std::array poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(imageCount)),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(imageCount))
    };

    auto const poolInfo = vk::DescriptorPoolCreateInfo({}, static_cast<uint32_t>(imageCount), poolSizes);

    auto createDescriptorPoolResult = m_device->createDescriptorPoolUnique(poolInfo);
    if (createDescriptorPoolResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating descriptor set", createDescriptorPoolResult);

    m_descriptorPool = std::move(createDescriptorPoolResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator & Gris::Graphics::Vulkan::VulkanDevice::Allocator()
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

const Gris::Graphics::Vulkan::VulkanAllocator & Gris::Graphics::Vulkan::VulkanDevice::Allocator() const
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool & Gris::Graphics::Vulkan::VulkanDevice::DescriptorPoolHandle() const
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool & Gris::Graphics::Vulkan::VulkanDevice::DescriptorPoolHandle()
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanShader Gris::Graphics::Vulkan::VulkanDevice::CreateShader(const std::vector<char> & code)
{
    return VulkanShader(this, code);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanSwapChain Gris::Graphics::Vulkan::VulkanDevice::CreateSwapChain(const VulkanWindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount)
{
    return VulkanSwapChain(this, window, width, height, virtualFrameCount);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanDeferredContext Gris::Graphics::Vulkan::VulkanDevice::CreateDeferredContext()
{
    return VulkanDeferredContext(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanBuffer Gris::Graphics::Vulkan::VulkanDevice::CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
{
    return VulkanBuffer(this, size, usage, properties);
}

// -------------------------------------------------------------------------------------------------
[[nodiscard]] Gris::Graphics::Vulkan::VulkanTexture Gris::Graphics::Vulkan::VulkanDevice::CreateTexture(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
{
    return VulkanTexture(this, width, height, mipLevels, numSamples, format, tiling, usage, properties);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanTextureView Gris::Graphics::Vulkan::VulkanDevice::CreateTextureView(const VulkanTexture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels)
{
    return VulkanTextureView(this, image, format, aspectFlags, mipLevels);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanSampler Gris::Graphics::Vulkan::VulkanDevice::CreateSampler(float minLod, float maxLod)
{
    return VulkanSampler(this, minLod, maxLod);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanPipelineStateObject Gris::Graphics::Vulkan::VulkanDevice::CreatePipelineStateObject(uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass & renderPass, const VulkanInputLayout & inputLayout)
{
    return VulkanPipelineStateObject(this, swapChainWidth, swapChainHeight, renderPass, inputLayout);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanFramebuffer Gris::Graphics::Vulkan::VulkanDevice::CreateFramebuffer(const VulkanTextureView & colorImageView, const VulkanTextureView & depthImageView, const VulkanTextureView & swapChainImageView, const VulkanRenderPass & renderPass, uint32_t width, uint32_t height)
{
    return VulkanFramebuffer(this, colorImageView, depthImageView, swapChainImageView, renderPass, width, height);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanFence Gris::Graphics::Vulkan::VulkanDevice::CreateFence(bool signaled)
{
    return VulkanFence(this, signaled);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanSemaphore Gris::Graphics::Vulkan::VulkanDevice::CreateSemaphore()
{
    return VulkanSemaphore(this);
}
