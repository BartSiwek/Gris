#include <gris/graphics/vulkan/shader_resource_bindings.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindings::ShaderResourceBindings() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindings::ShaderResourceBindings(
    const ParentObject<Device> & device,
    const ParentObject<ShaderResourceBindingsLayout> & resourceLayout)
    : DeviceResource(device)
    , ChildObject<ShaderResourceBindingsLayout>(resourceLayout)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindings::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::ShaderResourceBindings::IsValid() const
{
    return DeviceResource::IsValid() && ChildObject<ShaderResourceBindingsLayout>::IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBindings::DescriptorSetHandle() const
{
    return m_descriptorSet;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBindings::DescriptorSetHandle()
{
    return m_descriptorSet;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::ForeceRebuild()
{
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::SetSampler(std::string_view semantic, const Sampler & sampler)
{
    m_samplers[std::string{ semantic }] = &sampler;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::SetImageView(std::string_view semantic, const TextureView & textureView)
{
    m_textureViews[std::string{ semantic }] = &textureView;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::SetUniformBuffer(std::string_view semantic, const BufferView & bufferView)
{
    m_bufferViews[std::string{ semantic }] = &bufferView;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::SetCombinedSamplerAndImageView(std::string_view semantic, const Sampler & sampler, const TextureView & textureView)
{
    m_combinedSamplers[std::string{ semantic }] = CombinedSampler{ &sampler, &textureView };
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::PrepareBindings(Backend::ShaderResourceBindingsPoolCategory category, ShaderResourceBindingsPoolCollection * pools)
{
    if (!m_needsRebuilding)
    {
        return;
    }

    // Since the layout does not change during the lifetime of this object
    // the descriptor set is constant as well - only the contents may need updating
    // Consider adding a generation to ParentObject/ChildObject for change tracking (atomic)
    m_descriptorSet = pools->Allocate(category, ChildObject<ShaderResourceBindingsLayout>::Parent().DescriptorSetLayoutHandle());    

    auto descriptorCount = m_samplers.size() + m_textureViews.size() + m_bufferViews.size() + m_combinedSamplers.size();
    auto bufferInfos = MakeReservedVector<vk::DescriptorBufferInfo>(descriptorCount);
    auto imageInfos = MakeReservedVector<vk::DescriptorImageInfo>(descriptorCount);
    auto descriptorWrites = MakeReservedVector<vk::WriteDescriptorSet>(descriptorCount);

    for (auto const & [name, bufferView] : m_bufferViews)
    {
        bufferInfos.emplace_back(vk::DescriptorBufferInfo{}
                                     .setBuffer(bufferView->BufferHandle())
                                     .setOffset(bufferView->Offset())
                                     .setRange(bufferView->Size()));

        auto const & binding = ChildObject<ShaderResourceBindingsLayout>::Parent().NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                          .setBufferInfo(bufferInfos.back()));
    }

    for (auto const & [name, sampler] : m_samplers)
    {
        imageInfos.emplace_back(vk::DescriptorImageInfo{}.setSampler(sampler->SamplerHandle()));

        auto const & binding = ChildObject<ShaderResourceBindingsLayout>::Parent().NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eSampler)
                                          .setImageInfo(imageInfos.back()));
    }

    for (auto const & [name, textureView] : m_textureViews)
    {
        imageInfos.emplace_back(vk::DescriptorImageInfo{}
                                    .setImageView(textureView->ImageViewHandle())
                                    .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal));

        auto const & binding = ChildObject<ShaderResourceBindingsLayout>::Parent().NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eSampledImage)
                                          .setImageInfo(imageInfos.back()));
    }

    for (auto const & [name, samperAndTextureView] : m_combinedSamplers)
    {
        imageInfos.emplace_back(vk::DescriptorImageInfo{}
                                    .setSampler(samperAndTextureView.SamplerPart->SamplerHandle())
                                    .setImageView(samperAndTextureView.TextureViewPart->ImageViewHandle())
                                    .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal));

        auto const & binding = ChildObject<ShaderResourceBindingsLayout>::Parent().NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                          .setImageInfo(imageInfos.back()));
    }

    DeviceHandle().updateDescriptorSets(descriptorWrites, {}, Dispatch());

    m_needsRebuilding = false;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBindings::Reset()
{
    m_descriptorSet = nullptr;

    m_combinedSamplers.clear();
    m_bufferViews.clear();
    m_textureViews.clear();
    m_samplers.clear();

    m_needsRebuilding = true;

    ChildObject<ShaderResourceBindingsLayout>::ResetParent();
    DeviceResource::ResetParent();
}
