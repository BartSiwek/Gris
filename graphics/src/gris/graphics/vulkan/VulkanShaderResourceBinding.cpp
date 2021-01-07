#include <gris/graphics/vulkan/VulkanShaderResourceBinding.h>

#include <gris/graphics/vulkan/VulkanSampler.h>
#include <gris/graphics/vulkan/VulkanTextureView.h>
#include <gris/graphics/vulkan/VulkanBufferView.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanShaderResourceBinding::VulkanShaderResourceBinding(VulkanPipelineStateObject* pso, uint32_t frameCount) : VulkanPipelineStateObjectResource(pso)
{
    m_samplers.resize(frameCount);
    m_textureViews.resize(frameCount);
    m_bufferViews.resize(frameCount);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSet& Gris::Graphics::Vulkan::VulkanShaderResourceBinding::DescriptorSetHandle(uint32_t frameIndex) const
{
    return m_descriptorSets[frameIndex];
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSet& Gris::Graphics::Vulkan::VulkanShaderResourceBinding::DescriptorSetHandle(uint32_t frameIndex)
{
    return m_descriptorSets[frameIndex];
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanShaderResourceBinding::SetSampler(uint32_t frameIndex, const std::string& samplerName, const VulkanSampler& sampler)
{
    m_samplers[frameIndex][samplerName] = &sampler;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanShaderResourceBinding::SetImageView(uint32_t frameIndex, const std::string& imageName, const VulkanTextureView& textureView)
{
    m_textureViews[frameIndex][imageName] = &textureView;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanShaderResourceBinding::SetUniformBuffer(uint32_t frameIndex, const std::string& bufferName, const VulkanBufferView& bufferView)
{
    m_bufferViews[frameIndex][bufferName] = &bufferView;
}

// -------------------------------------------------------------------------------------------------

// TODO: Cook descriptor pool into device
void Gris::Graphics::Vulkan::VulkanShaderResourceBinding::CreateDescriptorSets() {
    auto const layoutCount = m_textureViews.size();
    std::vector<vk::DescriptorSetLayout> layouts(layoutCount, ParentPipelineStateObject().DescriptorSetLayoutHandle());
    auto const allocInfo = vk::DescriptorSetAllocateInfo(DescriptorPoolHandle(), layouts);

    auto allocateDescriptorSetsResult = DeviceHandle().allocateDescriptorSets(allocInfo);
    if (allocateDescriptorSetsResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating descriptor sets", allocateDescriptorSetsResult);

    m_descriptorSets = std::move(allocateDescriptorSetsResult.value);

    for (size_t i = 0; i < layoutCount; i++)
    {
        auto const& bufferView = m_bufferViews[i]["ubo"];
        std::array bufferInfo = { vk::DescriptorBufferInfo(bufferView->BufferHandle(), bufferView->Offset(), bufferView->Size()) };
        std::array imageInfo = { vk::DescriptorImageInfo(m_samplers[i]["texSampler"]->SamplerHandle(), m_textureViews[i]["texSampler"]->ImageViewHandle(), vk::ImageLayout::eShaderReadOnlyOptimal) };

        std::array descriptorWrites = {
            vk::WriteDescriptorSet(m_descriptorSets[i], 0, 0, vk::DescriptorType::eUniformBuffer, {}, bufferInfo, {}),
            vk::WriteDescriptorSet(m_descriptorSets[i], 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo, {}, {})
        };

        DeviceHandle().updateDescriptorSets(descriptorWrites, {});
    }
}
