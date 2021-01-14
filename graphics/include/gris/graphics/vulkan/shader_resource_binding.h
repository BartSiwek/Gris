#pragma once

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/pipeline_state_object_resource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanSampler;
class VulkanTextureView;
class VulkanBuffer;

class VulkanShaderResourceBinding : VulkanPipelineStateObjectResource
{
public:
    VulkanShaderResourceBinding(VulkanPipelineStateObject * pso, uint32_t frameCount);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSet & DescriptorSetHandle(uint32_t frameIndex) const;
    [[nodiscard]] vk::DescriptorSet & DescriptorSetHandle(uint32_t frameIndex);

    void SetSampler(uint32_t frameIndex, const std::string & samplerName, const VulkanSampler & sampler);
    void SetImageView(uint32_t frameIndex, const std::string & imageName, const VulkanTextureView & textureView);
    void SetUniformBuffer(uint32_t frameIndex, const std::string & bufferName, const VulkanBufferView & bufferView);

    void CreateDescriptorSets();

private:
    std::vector<std::unordered_map<std::string, const VulkanSampler *>> m_samplers;
    std::vector<std::unordered_map<std::string, const VulkanTextureView *>> m_textureViews;
    std::vector<std::unordered_map<std::string, const VulkanBufferView *>> m_bufferViews;

    std::vector<vk::DescriptorSet> m_descriptorSets;
};

}  // namespace Gris::Graphics::Vulkan
