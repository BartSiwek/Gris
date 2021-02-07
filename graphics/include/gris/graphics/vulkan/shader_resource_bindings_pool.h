#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

#include <optional>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsPool : DeviceResource
{
public:
    ShaderResourceBindingsPool(
        Device * device,
        Backend::ShaderResourceBindingsPoolCategory category,
        vk::UniqueDescriptorPool pool);

    ShaderResourceBindingsPool(const ShaderResourceBindingsPool &) = delete;
    ShaderResourceBindingsPool & operator=(const ShaderResourceBindingsPool &) = delete;

    ShaderResourceBindingsPool(ShaderResourceBindingsPool &&) = default;
    ShaderResourceBindingsPool & operator=(ShaderResourceBindingsPool &&) = default;

    ~ShaderResourceBindingsPool() = default;

    [[nodiscard]] Backend::ShaderResourceBindingsPoolCategory Category() const;

    [[nodiscard]] std::optional<vk::DescriptorSet> Allocate(const vk::DescriptorSetLayout & layout);

    void Reset();

private:
    Backend::ShaderResourceBindingsPoolCategory m_category;

    vk::UniqueDescriptorPool m_pool;
};

}  // namespace Gris::Graphics::Vulkan
