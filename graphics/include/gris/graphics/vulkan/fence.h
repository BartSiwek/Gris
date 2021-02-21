#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Fence : public DeviceResource
{
public:
    Fence();

    Fence(const ParentObject<Device> & device, bool signaled);

    Fence(const Fence &) = delete;
    Fence & operator=(const Fence &) = delete;

    Fence(Fence && other) noexcept;
    Fence & operator=(Fence && other) noexcept;

    virtual ~Fence();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Fence & FenceHandle() const;
    [[nodiscard]] vk::Fence & FenceHandle();

    void Reset();

private:
    vk::Fence m_fence = {};
};

}  // namespace Gris::Graphics::Vulkan
