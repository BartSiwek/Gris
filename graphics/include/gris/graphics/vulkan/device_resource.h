#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

namespace Gris::Graphics::Vulkan
{

class Device;
class Allocator;
class ShaderResourceBindingsPoolManager;

class DeviceResource
{
public:
    virtual ~DeviceResource() = default;

protected:
    explicit DeviceResource(Device * device);

    DeviceResource(const DeviceResource & other) = default;
    DeviceResource & operator=(const DeviceResource & other) = default;

    DeviceResource(DeviceResource && other) = default;
    DeviceResource & operator=(DeviceResource && other) = default;

    [[nodiscard]] const Device & ParentDevice() const;
    [[nodiscard]] Device & ParentDevice();

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const vk::DispatchLoaderDynamic & Dispatch() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

    [[nodiscard]] const ShaderResourceBindingsPoolManager & PoolManager(Backend::ShaderResourceBindingsPoolCategory category) const;
    [[nodiscard]] ShaderResourceBindingsPoolManager & PoolManager(Backend::ShaderResourceBindingsPoolCategory category);

private:
    Device * m_parentDevice = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
