#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

#include <optional>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsPool : DeviceResource
{
public:
    ShaderResourceBindingsPool();

    ShaderResourceBindingsPool(
        const ParentObject<Device> & device,
        Backend::ShaderResourceBindingsPoolCategory category,
        vk::DescriptorPool pool);

    ShaderResourceBindingsPool(const ShaderResourceBindingsPool &) = delete;
    ShaderResourceBindingsPool & operator=(const ShaderResourceBindingsPool &) = delete;

    ShaderResourceBindingsPool(ShaderResourceBindingsPool && other) noexcept;
    ShaderResourceBindingsPool & operator=(ShaderResourceBindingsPool && other) noexcept;

    virtual ~ShaderResourceBindingsPool();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] Backend::ShaderResourceBindingsPoolCategory Category() const;

    [[nodiscard]] std::optional<vk::DescriptorSet> Allocate(const vk::DescriptorSetLayout & layout);

    void ResetPool();

    void Reset();

private:
    void ReleaseResources();

    Backend::ShaderResourceBindingsPoolCategory m_category = {};
    vk::DescriptorPool m_pool = {};
};

}  // namespace Gris::Graphics::Vulkan
