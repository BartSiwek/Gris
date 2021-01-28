#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Sampler;
class TextureView;
class BufferView;
class Buffer;
class PipelineResourceGroupLayout;

class ShaderResourceBinding : DeviceResource
{
public:
    ShaderResourceBinding(Device * device);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSet & DescriptorSetHandle() const;
    [[nodiscard]] vk::DescriptorSet & DescriptorSetHandle();

    void SetSampler(const std::string & samplerName, const Sampler & sampler);
    void SetImageView(const std::string & imageName, const TextureView & textureView);
    void SetUniformBuffer(const std::string & bufferName, const BufferView & bufferView);

    void CreateDescriptorSets(const PipelineResourceGroupLayout & resourceLayout);

private:
    std::unordered_map<std::string, const Sampler *> m_samplers = {};
    std::unordered_map<std::string, const TextureView *> m_textureViews = {};
    std::unordered_map<std::string, const BufferView *> m_bufferViews = {};

    vk::DescriptorSet m_descriptorSet = {};
};

}  // namespace Gris::Graphics::Vulkan
