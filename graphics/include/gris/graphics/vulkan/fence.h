#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Fence : public DeviceResource
{
public:
    Fence(Device * device, bool signaled);

    // TODO: Do this better
    [[nodiscard]] const vk::Fence & FenceHandle() const;
    [[nodiscard]] vk::Fence & FenceHandle();

private:
    vk::UniqueFence m_fence;
};

}  // namespace Gris::Graphics::Vulkan
