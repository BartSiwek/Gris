#pragma once

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/pipeline_state_object_resource.h>

namespace Gris::Graphics::Vulkan
{

class Sampler;
class TextureView;
class Buffer;

class ShaderResourceBinding : PipelineStateObjectResource
{
public:
    ShaderResourceBinding(PipelineStateObject * pso, uint32_t frameCount);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSet & DescriptorSetHandle(uint32_t frameIndex) const;
    [[nodiscard]] vk::DescriptorSet & DescriptorSetHandle(uint32_t frameIndex);

    void SetSampler(uint32_t frameIndex, const std::string & samplerName, const Sampler & sampler);
    void SetImageView(uint32_t frameIndex, const std::string & imageName, const TextureView & textureView);
    void SetUniformBuffer(uint32_t frameIndex, const std::string & bufferName, const BufferView & bufferView);

    void CreateDescriptorSets();

private:
    std::vector<std::unordered_map<std::string, const Sampler *>> m_samplers;
    std::vector<std::unordered_map<std::string, const TextureView *>> m_textureViews;
    std::vector<std::unordered_map<std::string, const BufferView *>> m_bufferViews;

    std::vector<vk::DescriptorSet> m_descriptorSets;
};

}  // namespace Gris::Graphics::Vulkan
