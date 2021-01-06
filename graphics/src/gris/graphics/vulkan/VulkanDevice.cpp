#include <gris/graphics/vulkan/VulkanDevice.h>

#include <gris/graphics/vulkan/VulkanPipelineStateObject.h>
#include <gris/graphics/vulkan/VulkanFramebuffer.h>
#include <gris/graphics/vulkan/VulkanImmediateContext.h>
#include <gris/graphics/vulkan/VulkanDeferredContext.h>
#include <gris/graphics/vulkan/VulkanShader.h>
#include <gris/graphics/vulkan/VulkanBuffer.h>
#include <gris/graphics/vulkan/VulkanTexture.h>
#include <gris/graphics/vulkan/VulkanTextureView.h>
#include <gris/graphics/vulkan/VulkanSampler.h>
#include <gris/graphics/vulkan/VulkanFence.h>
#include <gris/graphics/vulkan/VulkanSemaphore.h>
#include <gris/graphics/vulkan/VulkanSwapChain.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanDevice::VulkanDevice(VulkanPhysicalDevice physicalDevice) : m_physicalDevice(std::move(physicalDevice))
{
    m_device = m_physicalDevice.CreateDevice();
    m_allocator = m_physicalDevice.CreateVulkanAllocator(m_device.get());
    m_context = std::make_unique<VulkanImmediateContext>(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const VulkanImmediateContext* VulkanDevice::Context() const
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanImmediateContext* VulkanDevice::Context()
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::SampleCountFlagBits& VulkanDevice::MsaaSamples() const
{
    return m_physicalDevice.MsaaSamples();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const DeviceQueueFamilyIndices& VulkanDevice::QueueFamilies() const
{
    return m_physicalDevice.QueueFamilies();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] SwapChainSupportDetails VulkanDevice::SwapChainSupport(const VulkanWindowMixin& window) const
{
    return m_physicalDevice.SwapChainSupport(window);
}

// -------------------------------------------------------------------------------------------------

void VulkanDevice::WaitIdle()
{
    auto const waitResult = m_device->waitIdle();
    if (waitResult != vk::Result::eSuccess)
        throw VulkanEngineException("Idle wait failed", waitResult);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format VulkanDevice::FindSupportedFormat(const std::vector<vk::Format>& candidates, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& features) const
{
    return m_physicalDevice.FindSupportedFormat(candidates, tiling, features);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::FormatProperties VulkanDevice::GetFormatProperties(vk::Format format) const
{
    return m_physicalDevice.GetFormatProperties(format);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Device& VulkanDevice::DeviceHandle() const
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Device& VulkanDevice::DeviceHandle()
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

void VulkanDevice::CreateDescriptorPool(uint32_t imageCount)
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

VulkanAllocator& VulkanDevice::Allocator()
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

const VulkanAllocator& VulkanDevice::Allocator() const
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool& VulkanDevice::DescriptorPoolHandle() const
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool& VulkanDevice::DescriptorPoolHandle()
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanShader VulkanDevice::CreateShader(const std::vector<char>& code)
{
    return VulkanShader(this, code);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanSwapChain VulkanDevice::CreateSwapChain(const VulkanWindowMixin& window, uint32_t width, uint32_t height, uint32_t virtualFrameCount)
{
    return VulkanSwapChain(this, window, width, height, virtualFrameCount);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanDeferredContext VulkanDevice::CreateDeferredContext()
{
    return VulkanDeferredContext(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanBuffer VulkanDevice::CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties)
{
    return VulkanBuffer(this, size, usage, properties);
}

// -------------------------------------------------------------------------------------------------
[[nodiscard]] VulkanTexture VulkanDevice::CreateTexture(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties)
{
    return VulkanTexture(this, width, height, mipLevels, numSamples, format, tiling, usage, properties);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanTextureView VulkanDevice::CreateTextureView(const VulkanTexture& image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels)
{
    return VulkanTextureView(this, image, format, aspectFlags, mipLevels);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanSampler VulkanDevice::CreateSampler(float minLod, float maxLod)
{
    return VulkanSampler(this, minLod, maxLod);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanPipelineStateObject VulkanDevice::CreatePipelineStateObject(uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass& renderPass, const VulkanInputLayout& inputLayout)
{
    return VulkanPipelineStateObject(this, swapChainWidth, swapChainHeight, renderPass, inputLayout);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanFramebuffer VulkanDevice::CreateFramebuffer(const VulkanTextureView& colorImageView, const VulkanTextureView& depthImageView, const VulkanTextureView& swapChainImageView, const VulkanRenderPass& renderPass, uint32_t width, uint32_t height)
{
    return VulkanFramebuffer(this, colorImageView, depthImageView, swapChainImageView, renderPass, width, height);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanFence VulkanDevice::CreateFence(bool signaled)
{
    return VulkanFence(this, signaled);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanSemaphore VulkanDevice::CreateSemaphore()
{
    return VulkanSemaphore(this);
}
