#include <gris/graphics/vulkan/shader_resource_bindings_pool.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ShaderResourceBindingsPool() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ShaderResourceBindingsPool(
    const ParentObject<Device> & device,
    Backend::ShaderResourceBindingsPoolCategory category,
    vk::DescriptorPool pool)
    : DeviceResource(device)
    , m_category(category)
    , m_pool(pool)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ShaderResourceBindingsPool(ShaderResourceBindingsPool && other) noexcept
    : DeviceResource(std::move(other))
    , m_category(std::exchange(other.m_category, {}))
    , m_pool(std::exchange(other.m_pool, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool & Gris::Graphics::Vulkan::ShaderResourceBindingsPool::operator=(ShaderResourceBindingsPool && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        DeviceResource::operator=(std::move(static_cast<DeviceResource &&>(other)));
        m_category = std::exchange(other.m_category, {});
        m_pool = std::exchange(other.m_pool, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::~ShaderResourceBindingsPool()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::ShaderResourceBindingsPool::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_pool);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory Gris::Graphics::Vulkan::ShaderResourceBindingsPool::Category() const
{
    return m_category;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<vk::DescriptorSet> Gris::Graphics::Vulkan::ShaderResourceBindingsPool::Allocate(const vk::DescriptorSetLayout & layout)
{
    auto layouts = std::array{ layout };
    auto allocInfo = vk::DescriptorSetAllocateInfo{}
                         .setSetLayouts(layouts)
                         .setDescriptorPool(m_pool)
                         .setDescriptorSetCount(1);

    auto allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo, Dispatch());

    switch (allocateDescriptorSetsResult.result)
    {
    case vk::Result::eSuccess:
        GRIS_ALWAYS_ASSERT(allocateDescriptorSetsResult.value.size() == 1, "Allocate descriptor sets allocated more than one");
        return allocateDescriptorSetsResult.value.front();
    case vk::Result::eErrorFragmentedPool:
    case vk::Result::eErrorOutOfPoolMemory:
        return {};
    default:
        throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ResetPool()
{
    DeviceHandle().resetDescriptorPool(m_pool, {}, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPool::Reset()
{
    ReleaseResources();
    m_category = {};
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ReleaseResources()
{
    if (m_pool)
    {
        DeviceHandle().destroyDescriptorPool(m_pool, nullptr, Dispatch());
        m_pool = nullptr;
    }
}
