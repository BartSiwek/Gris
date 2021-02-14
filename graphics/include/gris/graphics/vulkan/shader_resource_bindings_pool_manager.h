#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool.h>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsPoolManager : DeviceResource
{
public:
    ShaderResourceBindingsPoolManager();

    ShaderResourceBindingsPoolManager(
        const ParentObject<Device> & device,
        Backend::ShaderResourceBindingsPoolCategory category,
        const Backend::ShaderResourceBindingsPoolSizes & sizes);

    ShaderResourceBindingsPoolManager(const ShaderResourceBindingsPoolManager &) = delete;
    ShaderResourceBindingsPoolManager & operator=(const ShaderResourceBindingsPoolManager &) = delete;

    ShaderResourceBindingsPoolManager(ShaderResourceBindingsPoolManager &&) noexcept = default;
    ShaderResourceBindingsPoolManager & operator=(ShaderResourceBindingsPoolManager &&) noexcept = default;

    ~ShaderResourceBindingsPoolManager() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] Backend::ShaderResourceBindingsPoolCategory Category() const;

    void Update(const Backend::ShaderResourceBindingsPoolSizes & sizes);

    [[nodiscard]] ShaderResourceBindingsPool AllocatePool();
    void DeallocatePool(ShaderResourceBindingsPool pool);

private:
    Backend::ShaderResourceBindingsPoolCategory m_category = {};
    Backend::ShaderResourceBindingsPoolSizes m_sizes = {};
    std::vector<ShaderResourceBindingsPool> m_freePools = {};
};

}  // namespace Gris::Graphics::Vulkan
