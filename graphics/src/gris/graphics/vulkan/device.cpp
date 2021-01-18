#include <gris/graphics/vulkan/device.h>

#include <gris/graphics/vulkan/buffer.h>
#include <gris/graphics/vulkan/deferred_context.h>
#include <gris/graphics/vulkan/fence.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/semaphore.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::Device(PhysicalDevice physicalDevice)
    : m_physicalDevice(physicalDevice)
{
    m_device = m_physicalDevice.CreateDevice();
    m_allocator = m_physicalDevice.CreateAllocator(m_device.get());
    m_context = std::make_unique<ImmediateContext>(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::ImmediateContext * Gris::Graphics::Vulkan::Device::Context() const
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ImmediateContext * Gris::Graphics::Vulkan::Device::Context()
{
    return m_context.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::SampleCountFlagBits & Gris::Graphics::Vulkan::Device::MsaaSamples() const
{
    return m_physicalDevice.MsaaSamples();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::DeviceQueueFamilyIndices & Gris::Graphics::Vulkan::Device::QueueFamilies() const
{
    return m_physicalDevice.QueueFamilies();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::Device::SwapChainSupport(const WindowMixin & window) const
{
    return m_physicalDevice.SwapChainSupport(window);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Device::WaitIdle()
{
    auto const waitResult = m_device->waitIdle();
    if (waitResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Idle wait failed", waitResult);
    }
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format Gris::Graphics::Vulkan::Device::FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const
{
    return m_physicalDevice.FindSupportedFormat(candidates, tiling, features);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::FormatProperties Gris::Graphics::Vulkan::Device::GetFormatProperties(vk::Format format) const
{
    return m_physicalDevice.GetFormatProperties(format);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Device & Gris::Graphics::Vulkan::Device::DeviceHandle() const
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Device & Gris::Graphics::Vulkan::Device::DeviceHandle()
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Device::CreateDescriptorPool(uint32_t imageCount)
{
    std::array poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, imageCount),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, imageCount)
    };

    auto const poolInfo = vk::DescriptorPoolCreateInfo({}, imageCount, poolSizes);

    auto createDescriptorPoolResult = m_device->createDescriptorPoolUnique(poolInfo);
    if (createDescriptorPoolResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating descriptor set", createDescriptorPoolResult);
    }

    m_descriptorPool = std::move(createDescriptorPoolResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::Device::AllocatorHandle()
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::Device::AllocatorHandle() const
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool & Gris::Graphics::Vulkan::Device::DescriptorPoolHandle() const
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool & Gris::Graphics::Vulkan::Device::DescriptorPoolHandle()
{
    return m_descriptorPool.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Shader Gris::Graphics::Vulkan::Device::CreateShader(const std::vector<uint32_t> & code)
{
    return Shader(this, code);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChain Gris::Graphics::Vulkan::Device::CreateSwapChain(const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount)
{
    return SwapChain(this, window, width, height, virtualFrameCount);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::DeferredContext Gris::Graphics::Vulkan::Device::CreateDeferredContext()
{
    return DeferredContext(this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Buffer Gris::Graphics::Vulkan::Device::CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
{
    return Buffer(this, size, usage, properties);
}

// -------------------------------------------------------------------------------------------------
[[nodiscard]] Gris::Graphics::Vulkan::Texture Gris::Graphics::Vulkan::Device::CreateTexture(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
{
    return Texture(this, width, height, mipLevels, numSamples, format, tiling, usage, properties);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::TextureView Gris::Graphics::Vulkan::Device::CreateTextureView(const Texture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels)
{
    return TextureView(this, image, format, aspectFlags, mipLevels);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Sampler Gris::Graphics::Vulkan::Device::CreateSampler(float minLod, float maxLod)
{
    return Sampler(this, minLod, maxLod);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::PipelineStateObject Gris::Graphics::Vulkan::Device::CreatePipelineStateObject(uint32_t swapChainWidth, uint32_t swapChainHeight, const RenderPass & renderPass, const InputLayout & inputLayout, const Shader & vertexShader, const Shader & fragmentShader)
{
    return PipelineStateObject(this, swapChainWidth, swapChainHeight, renderPass, inputLayout, vertexShader, fragmentShader);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Framebuffer Gris::Graphics::Vulkan::Device::CreateFramebuffer(const TextureView & colorImageView, const TextureView & depthImageView, const TextureView & swapChainImageView, const RenderPass & renderPass, uint32_t width, uint32_t height)
{
    return Framebuffer(this, colorImageView, depthImageView, swapChainImageView, renderPass, width, height);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Fence Gris::Graphics::Vulkan::Device::CreateFence(bool signaled)
{
    return Fence(this, signaled);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Semaphore Gris::Graphics::Vulkan::Device::CreateSemaphore()
{
    return Semaphore(this);
}
