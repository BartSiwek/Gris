﻿#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

namespace Gris::Graphics::Vulkan
{

class Sampler;
class TextureView;
class BufferView;
class Buffer;
class ShaderResourceBindingsLayout;
class ShaderResourceBindingsPoolCollection;

class ShaderResourceBindings : public DeviceResource, private ChildObject<ShaderResourceBindingsLayout>
{
public:
    ShaderResourceBindings();

    ShaderResourceBindings(const ParentObject<Device> & device, const ParentObject<ShaderResourceBindingsLayout> & resourceLayout);

    ShaderResourceBindings(const ShaderResourceBindings &) = delete;
    ShaderResourceBindings & operator=(const ShaderResourceBindings &) = delete;

    ShaderResourceBindings(ShaderResourceBindings &&) noexcept = default;
    ShaderResourceBindings & operator=(ShaderResourceBindings &&) noexcept = default;

    virtual ~ShaderResourceBindings() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::DescriptorSet & DescriptorSetHandle() const;
    [[nodiscard]] vk::DescriptorSet & DescriptorSetHandle();

    void ForeceRebuild();

    void SetSampler(std::string_view semantic, const Sampler & sampler);
    void SetImageView(std::string_view semantic, const TextureView & textureView);
    void SetUniformBuffer(std::string_view semantic, const BufferView & bufferView);
    void SetCombinedSamplerAndImageView(std::string_view semantic, const Sampler & sampler, const TextureView & textureView);

    void PrepareBindings(Backend::ShaderResourceBindingsPoolCategory category, ShaderResourceBindingsPoolCollection * pools);

    void Reset();

private:
    struct CombinedSampler
    {
        const Sampler * SamplerPart;
        const TextureView * TextureViewPart;
    };

    bool m_needsRebuilding = true;

    std::unordered_map<std::string, const Sampler *> m_samplers = {};
    std::unordered_map<std::string, const TextureView *> m_textureViews = {};
    std::unordered_map<std::string, const BufferView *> m_bufferViews = {};
    std::unordered_map<std::string, CombinedSampler> m_combinedSamplers = {};

    vk::DescriptorSet m_descriptorSet = {};
};

}  // namespace Gris::Graphics::Vulkan
