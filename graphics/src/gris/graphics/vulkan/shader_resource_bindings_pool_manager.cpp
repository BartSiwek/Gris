#include <gris/graphics/vulkan/shader_resource_bindings_pool_manager.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

namespace
{

vk::UniqueDescriptorPool CreateNewPool(
    const vk::Device & device,
    const vk::DispatchLoaderDynamic & dispatch,
    const Gris::Graphics::Backend::ShaderResourceBindingsPoolSizes & sizes,
    vk::DescriptorPoolCreateFlags flags)
{
    auto poolSizes = Gris::MakeReservedVector<vk::DescriptorPoolSize>(Gris::Graphics::Backend::ShaderResourceBindingsPoolSizes::FactorCount);

    if(sizes.SamplerCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eSampler, sizes.SamplerCount);
    }
    if(sizes.CombinedImageSamplerCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, sizes.CombinedImageSamplerCount);
    }
    if(sizes.SampledImageCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eSampledImage, sizes.SampledImageCount);
    }
    if(sizes.StorageImageCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageImage, sizes.StorageImageCount);
    }
    if(sizes.UniformTexelBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformTexelBuffer, sizes.UniformTexelBufferCount);
    }
    if(sizes.StorageTexelBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageTexelBuffer, sizes.StorageTexelBufferCount);
    }
    if(sizes.UniformBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformBuffer, sizes.UniformBufferCount);
    }
    if(sizes.StorageBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageBuffer, sizes.StorageBufferCount);
    }
    if(sizes.DynamicUniformBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformBufferDynamic, sizes.DynamicUniformBufferCount);
    }
    if(sizes.DynamicStorageBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageBufferDynamic, sizes.DynamicStorageBufferCount);
    }
    if(sizes.InputAttachmentCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eInputAttachment, sizes.InputAttachmentCount);
    }

    auto createInfo = vk::DescriptorPoolCreateInfo{}
                          .setFlags(flags)
                          .setMaxSets(sizes.ShaderResourceBindingsCount)
                          .setPoolSizeCount(static_cast<uint32_t>(Gris::Graphics::Backend::ShaderResourceBindingsPoolSizes::FactorCount))
                          .setPoolSizes(poolSizes);

    auto createDescriptorPoolResult = device.createDescriptorPoolUnique(createInfo, nullptr, dispatch);
    if (createDescriptorPoolResult.result != vk::Result::eSuccess)
    {
        throw Gris::Graphics::Vulkan::VulkanEngineException("Error creating descriptor set", createDescriptorPoolResult);
    }


    return std::move(createDescriptorPoolResult.value);
}

}  // namespace

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::ShaderResourceBindingsPoolManager(Device * device, const Backend::ShaderResourceBindingsPoolSizes & sizes)
    : DeviceResource(device)
    , m_sizes(sizes)
{
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorSet Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::Allocate(const vk::DescriptorSetLayout & layout)
{
    if (!m_currentPool)
    {
        m_usedPools.push_back(GetPool());
        m_currentPool = m_usedPools.back().get();
    }

    auto layouts = std::array{ layout };
    auto allocInfo = vk::DescriptorSetAllocateInfo{}
                         .setSetLayouts(layouts)
                         .setDescriptorPool(m_currentPool)
                         .setDescriptorSetCount(1);

    auto allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo, Dispatch());

    bool needNewPool = false;
    switch (allocateDescriptorSetsResult.result)
    {
    case vk::Result::eSuccess:
        GRIS_ALWAYS_ASSERT(allocateDescriptorSetsResult.value.size() == 1, "Allocate descriptor sets allocated more than one");
        return std::move(allocateDescriptorSetsResult.value.front());
    case vk::Result::eErrorFragmentedPool:
    case vk::Result::eErrorOutOfPoolMemory:
        needNewPool = true;
        break;
    default:
        throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);
    }

    if (needNewPool)
    {
        m_usedPools.push_back(GetPool());
        m_currentPool = m_usedPools.back().get();

        allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo, Dispatch());

        if (allocateDescriptorSetsResult.result != vk::Result::eSuccess)
        {
            throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);
        }
    }

    GRIS_ALWAYS_ASSERT(allocateDescriptorSetsResult.value.size() == 1, "Allocate descriptor sets allocated more than one");
    return std::move(allocateDescriptorSetsResult.value.front());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::ResetPools()
{
    for (auto & pool : m_usedPools)
    {
        DeviceHandle().resetDescriptorPool(pool.get(), {}, Dispatch());
    }

    std::move(m_usedPools.begin(), m_usedPools.end(), std::back_inserter(m_freePools));
    m_usedPools.clear();

    m_currentPool = nullptr;
}

// -------------------------------------------------------------------------------------------------

vk::UniqueDescriptorPool Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::GetPool()
{
    if (!m_freePools.empty())
    {
        auto pool = std::move(m_freePools.back());
        m_freePools.pop_back();
        return pool;
    }
    else
    {
        return CreateNewPool(DeviceHandle(), Dispatch(), m_sizes, {});
    }
}
