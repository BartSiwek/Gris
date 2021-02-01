#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsPoolManager : DeviceResource
{
public:
    ShaderResourceBindingsPoolManager(Device * device, const Backend::ShaderResourceBindingsPoolSizes & sizes);

    ShaderResourceBindingsPoolManager(const ShaderResourceBindingsPoolManager &) = delete;
    ShaderResourceBindingsPoolManager& operator=(const ShaderResourceBindingsPoolManager &) = delete;

    ShaderResourceBindingsPoolManager(ShaderResourceBindingsPoolManager &&) = default;
    ShaderResourceBindingsPoolManager & operator=(ShaderResourceBindingsPoolManager &&) = default;

    ~ShaderResourceBindingsPoolManager() = default;

    [[nodiscard]] vk::DescriptorSet Allocate(const vk::DescriptorSetLayout & layout);
    void ResetPools();

private:
    vk::UniqueDescriptorPool GetPool();

    vk::DescriptorPool m_currentPool = {};
    Backend::ShaderResourceBindingsPoolSizes m_sizes = {};
    std::vector<vk::UniqueDescriptorPool> m_usedPools = {};
    std::vector<vk::UniqueDescriptorPool> m_freePools = {};
};

}  // namespace Gris::Graphics::Vulkan
