#pragma once

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/physical_device.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_manager.h>

#include <gris/span.h>

namespace Gris::Graphics::Backend
{

struct ShaderResourceBindingsLayout;

}

namespace Gris::Graphics::Vulkan
{

class SwapChain;
class DeviceResource;
class Buffer;
class Texture;
class DeferredContext;
class Shader;
class InputLayout;
class ShaderResourceBindingsLayout;
class ShaderResourceBindings;
class RenderPass;
class PipelineStateObject;
class TextureView;
class Sampler;
class Framebuffer;
class Fence;
class Semaphore;

class Device
{
public:
    friend class DeviceResource;

    explicit Device(PhysicalDevice physicalDevice);

    [[nodiscard]] const ImmediateContext * Context() const;
    [[nodiscard]] ImmediateContext * Context();

    [[nodiscard]] const vk::SampleCountFlagBits & MsaaSamples() const;

    [[nodiscard]] const DeviceQueueFamilyIndices & QueueFamilies() const;

    [[nodiscard]] SwapChainSupportDetails SwapChainSupport(const WindowMixin & window) const;

    void WaitIdle();

    [[nodiscard]] vk::Format FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const;

    [[nodiscard]] vk::FormatProperties GetFormatProperties(vk::Format format) const;

    void RegisterShaderResourceBindingsPoolCategory(Backend::ShaderResourceBindingsPoolCategory category, const Backend::ShaderResourceBindingsPoolSizes & sizes);

    // TODO: Do this better
    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] SwapChain CreateSwapChain(const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount);

    [[nodiscard]] DeferredContext CreateDeferredContext();
    [[nodiscard]] Shader CreateShader(const std::vector<uint32_t> & code, std::string entryPoint);
    [[nodiscard]] Buffer CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties);
    [[nodiscard]] Texture CreateTexture(
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        vk::SampleCountFlagBits numSamples,
        vk::Format format,
        vk::ImageTiling tiling,
        const vk::ImageUsageFlags & usage,
        const vk::MemoryPropertyFlags & properties);
    [[nodiscard]] TextureView CreateTextureView(const Texture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels);
    [[nodiscard]] Sampler CreateSampler(float minLod, float maxLod);
    [[nodiscard]] ShaderResourceBindingsLayout CreateShaderResourceBindingsLayout(const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindings);
    [[nodiscard]] PipelineStateObject CreatePipelineStateObject(
        uint32_t swapChainWidth,
        uint32_t swapChainHeight,
        const RenderPass & renderPass,
        const InputLayout & inputLayout,
        const ShaderResourceBindingsLayout & resourceLayout,
        const Shader & vertexShader,
        const Shader & fragmentShader);
    [[nodiscard]] ShaderResourceBindings CreateShaderResourceBindings(const ShaderResourceBindingsLayout * resourceLayout);
    [[nodiscard]] Framebuffer CreateFramebuffer(
        const TextureView & colorImageView,
        const TextureView & depthImageView,
        const TextureView & swapChainImageView,
        const RenderPass & renderPass,
        uint32_t width,
        uint32_t height);
    [[nodiscard]] Fence CreateFence(bool signaled);
    [[nodiscard]] Semaphore CreateSemaphore();

private:
    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

    [[nodiscard]] const ShaderResourceBindingsPoolManager & PoolManager(Backend::ShaderResourceBindingsPoolCategory category) const;
    [[nodiscard]] ShaderResourceBindingsPoolManager & PoolManager(Backend::ShaderResourceBindingsPoolCategory category);

    PhysicalDevice m_physicalDevice;

    vk::UniqueDevice m_device = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    Allocator m_allocator = {};
    std::unordered_map<Backend::ShaderResourceBindingsPoolCategory, ShaderResourceBindingsPoolManager> m_poolManagers;
    std::unique_ptr<ImmediateContext> m_context = {};
};

}  // namespace Gris::Graphics::Vulkan