#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/shader_resource_bindings_pool.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

#include <optional>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsPoolCollection : DeviceResource
{
public:
    ShaderResourceBindingsPoolCollection(Device * device);

    [[nodiscard]] vk::DescriptorSet Allocate(
        Backend::ShaderResourceBindingsPoolCategory category,
        const vk::DescriptorSetLayout & layout);

    void Clear();

    void ResetAll();

    void DeallocateAll();

private:
    struct PoolAndCategory
    {
        Backend::ShaderResourceBindingsPoolCategory Category;
        ShaderResourceBindingsPool Pool;
    };

    std::vector<PoolAndCategory> m_pools = {};
};

}  // namespace Gris::Graphics::Vulkan
