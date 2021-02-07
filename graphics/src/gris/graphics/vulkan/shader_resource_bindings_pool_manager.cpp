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

    if (sizes.SamplerCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eSampler, sizes.SamplerCount);
    }
    if (sizes.CombinedImageSamplerCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, sizes.CombinedImageSamplerCount);
    }
    if (sizes.SampledImageCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eSampledImage, sizes.SampledImageCount);
    }
    if (sizes.StorageImageCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageImage, sizes.StorageImageCount);
    }
    if (sizes.UniformTexelBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformTexelBuffer, sizes.UniformTexelBufferCount);
    }
    if (sizes.StorageTexelBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageTexelBuffer, sizes.StorageTexelBufferCount);
    }
    if (sizes.UniformBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformBuffer, sizes.UniformBufferCount);
    }
    if (sizes.StorageBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageBuffer, sizes.StorageBufferCount);
    }
    if (sizes.DynamicUniformBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eUniformBufferDynamic, sizes.DynamicUniformBufferCount);
    }
    if (sizes.DynamicStorageBufferCount > 0)
    {
        poolSizes.emplace_back(vk::DescriptorType::eStorageBufferDynamic, sizes.DynamicStorageBufferCount);
    }
    if (sizes.InputAttachmentCount > 0)
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

Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::ShaderResourceBindingsPoolManager(
    Device * device,
    Backend::ShaderResourceBindingsPoolCategory category,
    const Backend::ShaderResourceBindingsPoolSizes & sizes)
    : DeviceResource(device)
    , m_category(category)
    , m_sizes(sizes)
{
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::Category() const
{
    return m_category;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::Update(const Backend::ShaderResourceBindingsPoolSizes & sizes)
{
    m_freePools.clear();
    m_sizes = sizes;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsPool Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::AllocatePool()
{
    if (!m_freePools.empty())
    {
        auto pool = std::move(m_freePools.back());
        m_freePools.pop_back();
        return pool;
    }
    else
    {
        auto descriptorPool = CreateNewPool(DeviceHandle(), Dispatch(), m_sizes, {});
        return ShaderResourceBindingsPool(&ParentDevice(), m_category, std::move(descriptorPool));
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager::DeallocatePool(ShaderResourceBindingsPool pool)
{
    GRIS_ALWAYS_ASSERT(pool.Category() == m_category, "Pool deallocated with incompatible category");
    pool.Reset();
    m_freePools.emplace_back(std::move(pool));
}
