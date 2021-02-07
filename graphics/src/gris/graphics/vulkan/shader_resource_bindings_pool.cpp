#include <gris/graphics/vulkan/shader_resource_bindings_pool.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ShaderResourceBindingsPool() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::ShaderResourceBindingsPool(
    Device * device,
    Backend::ShaderResourceBindingsPoolCategory category,
    vk::UniqueDescriptorPool pool)
    : DeviceResource(device)
    , m_category(category)
    , m_pool(std::move(pool))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPool::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::ShaderResourceBindingsPool::IsValid() const
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
                         .setDescriptorPool(m_pool.get())
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

void Gris::Graphics::Vulkan::ShaderResourceBindingsPool::Reset()
{
    DeviceHandle().resetDescriptorPool(m_pool.get(), {}, Dispatch());
}
