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
        std::shared_ptr<DeviceSharedData> sharedData,
        Backend::ShaderResourceBindingsPoolCategory category,
        vk::UniqueDescriptorPool pool);

    ShaderResourceBindingsPool(const ShaderResourceBindingsPool &) = delete;
    ShaderResourceBindingsPool & operator=(const ShaderResourceBindingsPool &) = delete;

    ShaderResourceBindingsPool(ShaderResourceBindingsPool &&) noexcept = default;
    ShaderResourceBindingsPool & operator=(ShaderResourceBindingsPool &&) noexcept = default;

    ~ShaderResourceBindingsPool() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] Backend::ShaderResourceBindingsPoolCategory Category() const;

    [[nodiscard]] std::optional<vk::DescriptorSet> Allocate(const vk::DescriptorSetLayout & layout);

    void Reset();

private:
    Backend::ShaderResourceBindingsPoolCategory m_category = Backend::ShaderResourceBindingsPoolCategory{ std::numeric_limits<Backend::ShaderResourceBindingsPoolCategory::UnderlyingType>::max() };

    vk::UniqueDescriptorPool m_pool = {};
};

}  // namespace Gris::Graphics::Vulkan
