#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanDevice;

class VulkanSemaphore : public VulkanDeviceResource
{
public:
    explicit VulkanSemaphore(VulkanDevice * device);

    // TODO: Do this better
    const vk::Semaphore & SemaphoreHandle() const;
    vk::Semaphore & SemaphoreHandle();

private:
    vk::UniqueSemaphore m_semaphore = {};
};

}  // namespace Gris::Graphics::Vulkan
