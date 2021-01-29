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
    ShaderResourceBinding(Device * device, const PipelineResourceGroupLayout * resourceLayout);

    [[nodiscard]] const vk::DescriptorSet & DescriptorSetHandle() const;
    [[nodiscard]] vk::DescriptorSet & DescriptorSetHandle();

    void ForeceRebuild();

    void SetSampler(std::string_view semantic, const Sampler & sampler);
    void SetImageView(std::string_view semantic, const TextureView & textureView);
    void SetUniformBuffer(std::string_view semantic, const BufferView & bufferView);
    void SetCombinedSamplerAndImageView(std::string_view semantic, const Sampler & sampler, const TextureView & textureView);

    void PrepareBindings();

private:
    struct CombinedSampler
    {
        const Sampler * Sampler;
        const TextureView * TextureView;
    };

    const PipelineResourceGroupLayout * m_layout = nullptr;
    bool m_needsRebuilding = true;

    std::unordered_map<std::string, const Sampler *> m_samplers = {};
    std::unordered_map<std::string, const TextureView *> m_textureViews = {};
    std::unordered_map<std::string, const BufferView *> m_bufferViews = {};
    std::unordered_map<std::string, CombinedSampler> m_combinedSamplers = {};

    vk::DescriptorSet m_descriptorSet = {};
};

}  // namespace Gris::Graphics::Vulkan
