#include <gris/graphics/vulkan/shader_resource_binding.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBinding::ShaderResourceBinding(PipelineStateObject * pso)
    : PipelineStateObjectResource(pso)
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

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetSampler(const std::string & samplerName, const Sampler & sampler)
{
    m_samplers[samplerName] = &sampler;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetImageView(const std::string & imageName, const TextureView & textureView)
{
    m_textureViews[imageName] = &textureView;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::ShaderResourceBinding::SetUniformBuffer(const std::string & bufferName, const BufferView & bufferView)
{
    m_bufferViews[bufferName] = &bufferView;
}

// -------------------------------------------------------------------------------------------------

// TODO: Cook descriptor pool into device
void Gris::Graphics::Vulkan::ShaderResourceBinding::CreateDescriptorSets()
{
    auto layouts = std::array{ ParentPipelineStateObject().DescriptorSetLayoutHandle() };
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

    auto const & bufferView = m_bufferViews["ubo"];
    std::array bufferInfo = { vk::DescriptorBufferInfo{}
                                    .setBuffer(bufferView->BufferHandle())
                                    .setOffset(bufferView->Offset())
                                    .setRange(bufferView->Size()) };
    std::array imageInfo = { vk::DescriptorImageInfo{}
                                    .setSampler(m_samplers["texSampler"]->SamplerHandle())
                                    .setImageView(m_textureViews["texSampler"]->ImageViewHandle())
                                    .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal) };

    std::array descriptorWrites = {
        vk::WriteDescriptorSet{}
            .setDstSet(m_descriptorSet)
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setBufferInfo(bufferInfo),
        vk::WriteDescriptorSet{}
            .setDstSet(m_descriptorSet)
            .setDstBinding(1)
            .setDstArrayElement(0)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setImageInfo(imageInfo),
    };

    DeviceHandle().updateDescriptorSets(descriptorWrites, {}, Dispatch());
}
