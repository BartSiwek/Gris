#include <gris/graphics/vulkan/device.h>

#include <gris/graphics/vulkan/buffer.h>
#include <gris/graphics/vulkan/deferred_context.h>
#include <gris/graphics/vulkan/fence.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/semaphore.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/shader_resource_bindings.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::Device() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::Device(PhysicalDevice physicalDevice)
    : m_physicalDevice(physicalDevice)
    , m_device(m_physicalDevice.CreateDevice())
    , m_sharedData(std::make_shared<DeviceSharedData>(Instance::CreateDispatch(m_device.get()), this))
    , m_allocator(m_physicalDevice.CreateAllocator(m_device.get(), m_sharedData->Dispatch))
    , m_context(m_sharedData)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::Device(Device && other) noexcept
    : m_physicalDevice(std::exchange(other.m_physicalDevice, {}))
    , m_device(std::exchange(other.m_device, {}))
    , m_sharedData(std::exchange(other.m_sharedData, {}))
    , m_allocator(std::exchange(other.m_allocator, {}))
    , m_context(std::exchange(other.m_context, {}))
    , m_poolManagers(std::exchange(other.m_poolManagers, {}))
{
    m_sharedData->ParentDevice = this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::Device::operator=(Device && other) noexcept
{
    if (&other != this)
    {
        m_physicalDevice = std::exchange(other.m_physicalDevice, {});
        m_device = std::exchange(other.m_device, {});
        m_sharedData = std::exchange(other.m_sharedData, {});
        m_allocator = std::exchange(other.m_allocator, {});
        m_context = std::exchange(other.m_context, {});
        m_poolManagers = std::exchange(other.m_poolManagers, {});

        m_sharedData->ParentDevice = this;
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::~Device()
{
    if (m_sharedData)
    {
        m_sharedData->ParentDevice = nullptr;
    }
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Device::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::Device::IsValid() const
{
    return m_physicalDevice.IsValid() && static_cast<bool>(m_device);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::ImmediateContext & Gris::Graphics::Vulkan::Device::Context() const
{
    return m_context;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ImmediateContext & Gris::Graphics::Vulkan::Device::Context()
{
    return m_context;
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
    auto const waitResult = m_device->waitIdle(m_sharedData->Dispatch);
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

void Gris::Graphics::Vulkan::Device::RegisterShaderResourceBindingsPoolCategory(
    Backend::ShaderResourceBindingsPoolCategory category,
    const Backend::ShaderResourceBindingsPoolSizes & sizes)
{
    GRIS_FAST_ASSERT(std::find_if(std::begin(m_poolManagers), std::end(m_poolManagers), [&category](const auto & entry)
                                  { return entry.Category == category; }),
                     "Registering an already registered pool category ");
    m_poolManagers.emplace_back(CategoryAndPoolManager{ category, { m_sharedData, category, sizes } });
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Device::UpdateShaderResourceBindingsPoolCategory(
    Backend::ShaderResourceBindingsPoolCategory category,
    const Backend::ShaderResourceBindingsPoolSizes & sizes)
{
    auto it = std::find_if(std::begin(m_poolManagers), std::end(m_poolManagers), [&category](const auto & entry)
                           { return entry.Category == category; });
    GRIS_ALWAYS_ASSERT(it != std::end(m_poolManagers), "Updating an non-existent pool category");
    it->PoolManager.Update(sizes);
}

// -------------------------------------------------------------------------------------------------

const vk::Device & Gris::Graphics::Vulkan::Device::DeviceHandle() const
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

vk::Device & Gris::Graphics::Vulkan::Device::DeviceHandle()
{
    return m_device.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Shader Gris::Graphics::Vulkan::Device::CreateShader(const std::vector<uint32_t> & code, std::string entryPoint)
{
    return Shader(m_sharedData, code, std::move(entryPoint));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChain Gris::Graphics::Vulkan::Device::CreateSwapChain(const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount)
{
    return SwapChain(m_sharedData, window, width, height, virtualFrameCount);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::DeferredContext Gris::Graphics::Vulkan::Device::CreateDeferredContext()
{
    return DeferredContext(m_sharedData);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Buffer Gris::Graphics::Vulkan::Device::CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
{
    return Buffer(m_sharedData, size, usage, properties);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Texture Gris::Graphics::Vulkan::Device::CreateTexture(
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    vk::SampleCountFlagBits numSamples,
    vk::Format format,
    vk::ImageTiling tiling,
    const vk::ImageUsageFlags & usage,
    const vk::MemoryPropertyFlags & properties)
{
    return Texture(m_sharedData, width, height, mipLevels, numSamples, format, tiling, usage, properties);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::TextureView Gris::Graphics::Vulkan::Device::CreateTextureView(const Texture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels)
{
    return TextureView(m_sharedData, image, format, aspectFlags, mipLevels);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Sampler Gris::Graphics::Vulkan::Device::CreateSampler(float minLod, float maxLod)
{
    return Sampler(m_sharedData, minLod, maxLod);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsLayout Gris::Graphics::Vulkan::Device::CreateShaderResourceBindingsLayout(const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindings)
{
    return ShaderResourceBindingsLayout(m_sharedData, bindings);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::PipelineStateObject Gris::Graphics::Vulkan::Device::CreatePipelineStateObject(
    uint32_t swapChainWidth,
    uint32_t swapChainHeight,
    const RenderPass & renderPass,
    const InputLayout & inputLayout,
    const ShaderResourceBindingsLayout & resourceLayout,
    const Shader & vertexShader,
    const Shader & fragmentShader)
{
    return PipelineStateObject(m_sharedData, swapChainWidth, swapChainHeight, renderPass, inputLayout, resourceLayout, vertexShader, fragmentShader);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindings Gris::Graphics::Vulkan::Device::CreateShaderResourceBindings(const ShaderResourceBindingsLayout * resourceLayout)
{
    return ShaderResourceBindings(m_sharedData, resourceLayout);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Framebuffer Gris::Graphics::Vulkan::Device::CreateFramebuffer(
    const TextureView & colorImageView,
    const TextureView & depthImageView,
    const TextureView & swapChainImageView,
    const RenderPass & renderPass,
    uint32_t width,
    uint32_t height)
{
    return Framebuffer(m_sharedData, colorImageView, depthImageView, swapChainImageView, renderPass, width, height);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Fence Gris::Graphics::Vulkan::Device::CreateFence(bool signaled)
{
    return Fence(m_sharedData, signaled);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Semaphore Gris::Graphics::Vulkan::Device::CreateSemaphore()
{
    return Semaphore(m_sharedData);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::RenderPass Gris::Graphics::Vulkan::Device::CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat)
{
    return RenderPass(m_sharedData, swapChainFormat, depthFormat);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection Gris::Graphics::Vulkan::Device::CreateShaderResourceBindingsPoolCollection()
{
    return ShaderResourceBindingsPoolCollection(m_sharedData);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsPool Gris::Graphics::Vulkan::Device::AllocateShaderResourceBindingsPool(Backend::ShaderResourceBindingsPoolCategory category)
{
    auto it = std::find_if(std::begin(m_poolManagers), std::end(m_poolManagers), [&category](const auto & entry)
                           { return entry.Category == category; });
    GRIS_ALWAYS_ASSERT(it != std::end(m_poolManagers), "Allocating a pool from an unknown descriptor pool category");
    return it->PoolManager.AllocatePool();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Device::DeallocateShaderResourceBindingsPool(ShaderResourceBindingsPool pool)
{
    auto it = std::find_if(std::begin(m_poolManagers), std::end(m_poolManagers), [&pool](const auto & entry)
                           { return entry.Category == pool.Category(); });
    GRIS_ALWAYS_ASSERT(it != std::end(m_poolManagers), "Deallocating a pool with a unknown descriptor pool category");
    it->PoolManager.DeallocatePool(std::move(pool));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::TextureView Gris::Graphics::Vulkan::Device::CreateTextureView(const vk::Image & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels)
{
    return TextureView(m_sharedData, image, format, aspectFlags, mipLevels);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsPool Gris::Graphics::Vulkan::Device::CreateShaderResourceBindingsPool(Backend::ShaderResourceBindingsPoolCategory category, vk::UniqueDescriptorPool pool)
{
    return ShaderResourceBindingsPool(m_sharedData, category, std::move(pool));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::Device::AllocatorHandle()
{
    return m_allocator;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::Device::AllocatorHandle() const
{
    return m_allocator;
}
