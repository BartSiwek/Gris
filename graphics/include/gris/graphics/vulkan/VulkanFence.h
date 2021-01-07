#pragma once

#include "VulkanDeviceResource.h"

namespace Gris::Graphics::Vulkan
{

class VulkanFence : public VulkanDeviceResource
{
public:
    VulkanFence(VulkanDevice * device, bool signaled);

    // TODO: Do this better
    [[nodiscard]] const vk::Fence & FenceHandle() const;
    [[nodiscard]] vk::Fence & FenceHandle();

private:
    vk::UniqueFence m_fence;
};

}  // namespace Gris::Graphics::Vulkan
