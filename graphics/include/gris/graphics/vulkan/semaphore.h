#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Device;

class Semaphore : public DeviceResource
{
public:
    explicit Semaphore(Device * device);

    const vk::Semaphore & SemaphoreHandle() const;
    vk::Semaphore & SemaphoreHandle();

private:
    vk::UniqueSemaphore m_semaphore = {};
};

}  // namespace Gris::Graphics::Vulkan
