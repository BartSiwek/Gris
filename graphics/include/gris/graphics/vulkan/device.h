#pragma once

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/physical_device.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_manager.h>

#include <gris/object_hierarchy.h>
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
class ShaderResourceBindingsPool;
class ShaderResourceBindingsPoolCollection;
class RenderPass;
class PipelineStateObject;
class TextureView;
class Sampler;
class Framebuffer;
class Fence;
class Semaphore;
class RenderPass;

class Device : public ParentObject<Device>
{
public:
    friend class DeviceResource;

    Device();

    explicit Device(PhysicalDevice physicalDevice);

    Device(const Device &) = delete;
    Device & operator=(const Device &) = delete;

    Device(Device && other) noexcept;
    Device & operator=(Device && other) noexcept;

    ~Device() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const ImmediateContext & Context() const;
    [[nodiscard]] ImmediateContext & Context();

    [[nodiscard]] const vk::SampleCountFlagBits & MsaaSamples() const;

    [[nodiscard]] const DeviceQueueFamilyIndices & QueueFamilies() const;

    [[nodiscard]] SwapChainSupportDetails SwapChainSupport(const WindowMixin & window) const;

    void WaitIdle() const;

    [[nodiscard]] vk::Format FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const;

    [[nodiscard]] vk::FormatProperties GetFormatProperties(vk::Format format) const;

    void RegisterShaderResourceBindingsPoolCategory(Backend::ShaderResourceBindingsPoolCategory category, const Backend::ShaderResourceBindingsPoolSizes & sizes);
    void UpdateShaderResourceBindingsPoolCategory(Backend::ShaderResourceBindingsPoolCategory category, const Backend::ShaderResourceBindingsPoolSizes & sizes);

    [[nodiscard]] SwapChain CreateSwapChain(const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount) const;
    [[nodiscard]] SwapChain CreateSwapChain(const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount, SwapChain oldSwapChain) const;
    [[nodiscard]] DeferredContext CreateDeferredContext(bool transientCommandBuffers) const;
    [[nodiscard]] Shader CreateShader(const std::vector<uint32_t> & code, std::string entryPoint) const;
    [[nodiscard]] Buffer CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties) const;
    [[nodiscard]] Texture CreateTexture(
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        vk::SampleCountFlagBits numSamples,
        vk::Format format,
        vk::ImageTiling tiling,
        const vk::ImageUsageFlags & usage,
        const vk::MemoryPropertyFlags & properties) const;
    [[nodiscard]] TextureView CreateTextureView(const Texture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels) const;
    [[nodiscard]] Sampler CreateSampler(float minLod, float maxLod) const;
    [[nodiscard]] ShaderResourceBindingsLayout CreateShaderResourceBindingsLayout(const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindings) const;
    [[nodiscard]] PipelineStateObject CreatePipelineStateObject(
        uint32_t swapChainWidth,
        uint32_t swapChainHeight,
        const RenderPass & renderPass,
        const InputLayout & inputLayout,
        Gris::Span<const ShaderResourceBindingsLayout> resourceLayouts,
        const Shader & vertexShader,
        const Shader & fragmentShader) const;
    [[nodiscard]] ShaderResourceBindings CreateShaderResourceBindings(const ParentObject<ShaderResourceBindingsLayout> & resourceLayout) const;
    [[nodiscard]] Framebuffer CreateFramebuffer(
        const TextureView & colorImageView,
        const TextureView & depthImageView,
        const TextureView & swapChainImageView,
        const RenderPass & renderPass,
        uint32_t width,
        uint32_t height) const;
    [[nodiscard]] Fence CreateFence(bool signaled) const;
    [[nodiscard]] Semaphore CreateSemaphore() const;
    [[nodiscard]] RenderPass CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat) const;
    [[nodiscard]] ShaderResourceBindingsPoolCollection CreateShaderResourceBindingsPoolCollection() const;
    [[nodiscard]] TextureView CreateTextureView(const vk::Image & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels) const;
    [[nodiscard]] ShaderResourceBindingsPool CreateShaderResourceBindingsPool(Backend::ShaderResourceBindingsPoolCategory category, vk::DescriptorPool pool) const;

    [[nodiscard]] ShaderResourceBindingsPool AllocateShaderResourceBindingsPool(Backend::ShaderResourceBindingsPoolCategory category);
    void DeallocateShaderResourceBindingsPool(ShaderResourceBindingsPool pool);

    void Reset();

private:
    struct CategoryAndPoolManager
    {
        Backend::ShaderResourceBindingsPoolCategory Category;
        ShaderResourceBindingsPoolManager PoolManager;
    };

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

    [[nodiscard]] const vk::DispatchLoaderDynamic & DispatchHandle() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & DispatchHandle();

    void ReleaseResources();

    PhysicalDevice m_physicalDevice = {};
    vk::Device m_device = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    Allocator m_allocator = {};
    ImmediateContext m_context = {};
    std::vector<CategoryAndPoolManager> m_poolManagers;
};

}  // namespace Gris::Graphics::Vulkan