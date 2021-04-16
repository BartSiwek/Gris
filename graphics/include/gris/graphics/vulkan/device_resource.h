#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/object_hierarchy.h>

namespace Gris::Graphics::Vulkan
{

class Device;
class Allocator;

class DeviceResource : ChildObject<Device>
{
public:
    ~DeviceResource() override = default;

protected:
    DeviceResource();

    explicit DeviceResource(const ParentObject<Device> & device);

    DeviceResource(const DeviceResource &) = default;
    DeviceResource & operator=(const DeviceResource &) = default;

    DeviceResource(DeviceResource &&) noexcept = default;
    DeviceResource & operator=(DeviceResource &&) noexcept = default;

    [[nodiscard]] bool IsDeviceValid() const;

    [[nodiscard]] const Device & ParentDevice() const;
    [[nodiscard]] Device & ParentDevice();

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const vk::DispatchLoaderDynamic & Dispatch() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] const Allocator & AllocatorHandle() const;
    [[nodiscard]] Allocator & AllocatorHandle();

    using ChildObject::ResetParent;

private:
    using ChildObject::IsParentValid;
};

}  // namespace Gris::Graphics::Vulkan
