#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/graphics/backend/shader_resource_bindings_pool_sizes.h>

#include <gris/object_hierarchy.h>

namespace Gris::Graphics::Vulkan
{

class Device;
class Allocator;

class DeviceResource : private ChildObject<Device>
{
public:
    virtual ~DeviceResource() = default;

protected:
    DeviceResource();

    explicit DeviceResource(const ParentObject<Device> & device);

    DeviceResource(const DeviceResource &) = default;
    DeviceResource & operator=(const DeviceResource &) = default;

    DeviceResource(DeviceResource &&) noexcept = default;
    DeviceResource & operator=(DeviceResource &&) noexcept = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const Device & ParentDevice() const;
    [[nodiscard]] Device & ParentDevice();

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const vk::DispatchLoaderDynamic & Dispatch() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

    using ChildObject::ResetParent;
};

}  // namespace Gris::Graphics::Vulkan
