#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/graphics/vulkan/device_shared_data.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

namespace Gris::Graphics::Vulkan
{

class Device;
class Allocator;

class DeviceResource
{
public:
    virtual ~DeviceResource() = default;

protected:
    DeviceResource();

    explicit DeviceResource(std::shared_ptr<DeviceSharedData> sharedData);

    DeviceResource(const DeviceResource &) = default;
    DeviceResource & operator=(const DeviceResource &) = default;

    DeviceResource(DeviceResource &&) noexcept = default;
    DeviceResource & operator=(DeviceResource &&) noexcept = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] const Device & ParentDevice() const;
    [[nodiscard]] Device & ParentDevice();

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const vk::DispatchLoaderDynamic & Dispatch() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

private:
    std::shared_ptr<DeviceSharedData> m_sharedData = {};
};

}  // namespace Gris::Graphics::Vulkan
