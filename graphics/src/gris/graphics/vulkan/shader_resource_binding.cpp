#include <gris/graphics/vulkan/shader_resource_binding.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/pipeline_resource_group_layout.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBinding::ShaderResourceBinding(Device * device)
    : DeviceResource(device)
{
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBinding::DescriptorSetHandle() const
{
    return m_descriptorSet;
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBinding::DescriptorSetHandle()
{
    return m_descriptorSet;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetSampler(std::string_view semantic, const Sampler & sampler)
{
    m_samplers[std::string{ semantic }] = &sampler;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetImageView(std::string_view semantic, const TextureView & textureView)
{
    m_textureViews[std::string{ semantic }] = &textureView;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetUniformBuffer(std::string_view semantic, const BufferView & bufferView)
{
    m_bufferViews[std::string{ semantic }] = &bufferView;
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetCombinedSamplerAndImageView(std::string_view semantic, const Sampler & sampler, const TextureView & textureView)
{
    m_combinedSamplers[std::string{ semantic }] = CombinedSampler{ &sampler, &textureView };
    m_needsRebuilding = true;
}

// -------------------------------------------------------------------------------------------------

// TODO: Cook descriptor pool into device
void Gris::Graphics::Vulkan::ShaderResourceBinding::CreateDescriptorSets(const PipelineResourceGroupLayout & resourceLayout)
{
    if (!m_needsRebuilding)
        return;

    auto layouts = std::array{ resourceLayout.DescriptorSetLayoutHandle() };
    auto const allocInfo = vk::DescriptorSetAllocateInfo{}
                               .setDescriptorPool(DescriptorPoolHandle())
                               .setSetLayouts(layouts);

    auto allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo, Dispatch());
    if (allocateDescriptorSetsResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);
    }

    GRIS_ALWAYS_ASSERT(allocateDescriptorSetsResult.value.size() == 1, "Number of allocated descriptor sets should be one");
    m_descriptorSet = std::move(allocateDescriptorSetsResult.value.front());

    auto descriptorCount = m_samplers.size() + m_textureViews.size() + m_bufferViews.size() + m_combinedSamplers.size();
    auto descriptorWrites = MakeReservedVector<vk::WriteDescriptorSet>(descriptorCount);

    for (auto const & [name, bufferView] : m_bufferViews)
    {
        auto bufferInfo = vk::DescriptorBufferInfo{}
                              .setBuffer(bufferView->BufferHandle())
                              .setOffset(bufferView->Offset())
                              .setRange(bufferView->Size());

        auto const & binding = resourceLayout.NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");  // TODO: Support arrays

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                          .setBufferInfo(bufferInfo));
    }

    for (auto const & [name, sampler] : m_samplers)
    {
        vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo{}.setSampler(sampler->SamplerHandle());

        auto const & binding = resourceLayout.NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");  // TODO: Support arrays

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eSampler)
                                          .setImageInfo(imageInfo));
    }

    for (auto const & [name, textureView] : m_textureViews)
    {
        vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo{}
                                                .setImageView(textureView->ImageViewHandle())
                                                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        auto const & binding = resourceLayout.NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");  // TODO: Support arrays

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eSampledImage)
                                          .setImageInfo(imageInfo));
    }

    for (auto const & [name, samperAndTextureView] : m_combinedSamplers)
    {
        vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo{}
                                                .setSampler(samperAndTextureView.Sampler->SamplerHandle())
                                                .setImageView(samperAndTextureView.TextureView->ImageViewHandle())
                                                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        auto const & binding = resourceLayout.NameToBinding(name);
        GRIS_ALWAYS_ASSERT(binding.descriptorCount == 1, "Descriptor arrays are not supported");  // TODO: Support arrays

        descriptorWrites.emplace_back(vk::WriteDescriptorSet{}
                                          .setDstSet(m_descriptorSet)
                                          .setDstBinding(binding.binding)
                                          .setDstArrayElement(0)
                                          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                          .setImageInfo(imageInfo));
    }

    DeviceHandle().updateDescriptorSets(descriptorWrites, {}, Dispatch());

    m_needsRebuilding = false;
}
