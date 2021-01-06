#pragma once

#include "VulkanDeviceResource.h"

class VulkanFence : public VulkanDeviceResource
{
public:
    VulkanFence(VulkanDevice* device, bool signaled);

    // TODO: Do this better
    [[nodiscard]] const vk::Fence& FenceHandle() const;
    [[nodiscard]] vk::Fence& FenceHandle();

private:
    vk::UniqueFence m_fence;
};
