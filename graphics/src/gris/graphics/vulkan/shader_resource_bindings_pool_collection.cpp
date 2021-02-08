#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::ShaderResourceBindingsPoolCollection() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::ShaderResourceBindingsPoolCollection(std::shared_ptr<DeviceSharedData> sharedData)
    : DeviceResource(sharedData)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::IsValid() const
{
    return DeviceResource::IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorSet Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::Allocate(
    Backend::ShaderResourceBindingsPoolCategory category,
    const vk::DescriptorSetLayout & layout)
{
    auto it = std::find_if(std::begin(m_pools), std::end(m_pools), [&category](const auto & entry)
                           { return entry.Category == category; });
    if (it == std::end(m_pools))
    {
        m_pools.emplace_back(PoolAndCategory{ category, ParentDevice().AllocateShaderResourceBindingsPool(category) });
        auto allocateResult = m_pools.back().Pool.Allocate(layout);
        if (!allocateResult)
        {
            throw VulkanEngineException("Cannot allocate shader bindings from pool");
        }

        return *allocateResult;
    }

    auto allocateResult = it->Pool.Allocate(layout);
    if (!allocateResult)
    {
        m_pools.emplace_back(PoolAndCategory{ category, ParentDevice().AllocateShaderResourceBindingsPool(category) });
        allocateResult = m_pools.back().Pool.Allocate(layout);
        if (!allocateResult)
        {
            throw VulkanEngineException("Cannot allocate shader bindings from pool");
        }
    }

    return *allocateResult;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::Clear()
{
    m_pools.clear();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::ResetAll()
{
    for (auto & entry : m_pools)
    {
        entry.Pool.Reset();
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection::DeallocateAll()
{
    for (auto & entry : m_pools)
    {
        ParentDevice().DeallocateShaderResourceBindingsPool(std::move(entry.Pool));
    }
    m_pools.clear();
}
