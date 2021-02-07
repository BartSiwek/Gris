﻿#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Fence : public DeviceResource
{
public:
    Fence();

    Fence(Device * device, bool signaled);

    Fence(const Fence &) = delete;
    Fence & operator=(const Fence &) = delete;

    Fence(Fence &&) noexcept = default;
    Fence & operator=(Fence &&) noexcept = default;

    ~Fence() = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] const vk::Fence & FenceHandle() const;
    [[nodiscard]] vk::Fence & FenceHandle();

private:
    vk::UniqueFence m_fence = {};
};

}  // namespace Gris::Graphics::Vulkan
