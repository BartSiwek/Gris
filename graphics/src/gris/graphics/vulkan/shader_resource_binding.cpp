#include <gris/graphics/vulkan/shader_resource_binding.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBinding::ShaderResourceBinding(PipelineStateObject * pso, uint32_t frameCount)
    : PipelineStateObjectResource(pso)
{
    m_samplers.resize(frameCount);
    m_textureViews.resize(frameCount);
    m_bufferViews.resize(frameCount);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBinding::DescriptorSetHandle(uint32_t frameIndex) const
{
    return m_descriptorSets[frameIndex];
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSet & Gris::Graphics::Vulkan::ShaderResourceBinding::DescriptorSetHandle(uint32_t frameIndex)
{
    return m_descriptorSets[frameIndex];
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetSampler(uint32_t frameIndex, const std::string & samplerName, const Sampler & sampler)
{
    m_samplers[frameIndex][samplerName] = &sampler;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetImageView(uint32_t frameIndex, const std::string & imageName, const TextureView & textureView)
{
    m_textureViews[frameIndex][imageName] = &textureView;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetUniformBuffer(uint32_t frameIndex, const std::string & bufferName, const BufferView & bufferView)
{
    m_bufferViews[frameIndex][bufferName] = &bufferView;
}

// -------------------------------------------------------------------------------------------------

// TODO: Cook descriptor pool into device
void Gris::Graphics::Vulkan::ShaderResourceBinding::CreateDescriptorSets()
{
    auto const layoutCount = m_textureViews.size();
    std::vector<vk::DescriptorSetLayout> layouts(layoutCount, ParentPipelineStateObject().DescriptorSetLayoutHandle());
    auto const allocInfo = vk::DescriptorSetAllocateInfo(DescriptorPoolHandle(), layouts);

    auto allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo);
    if (allocateDescriptorSetsResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);

    m_descriptorSets = std::move(allocateDescriptorSetsResult.value);

    for (size_t i = 0; i < layoutCount; i++)
    {
        auto const & bufferView = m_bufferViews[i]["ubo"];
        std::array bufferInfo = { vk::DescriptorBufferInfo(bufferView->BufferHandle(), bufferView->Offset(), bufferView->Size()) };
        std::array imageInfo = { vk::DescriptorImageInfo(m_samplers[i]["texSampler"]->SamplerHandle(), m_textureViews[i]["texSampler"]->ImageViewHandle(), vk::ImageLayout::eShaderReadOnlyOptimal) };

        std::array descriptorWrites = {
            vk::WriteDescriptorSet(m_descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, {}, bufferInfo, {}),
            vk::WriteDescriptorSet(m_descriptorSets[i], 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, {}, {})
        };

        DeviceHandle().updateDescriptorSets(descriptorWrites, {});
    }
}
