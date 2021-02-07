#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Device;

class Semaphore : public DeviceResource
{
public:
    Semaphore();

    explicit Semaphore(Device * device);

    Semaphore(const Semaphore &) = delete;
    Semaphore & operator=(const Semaphore &) = delete;

    Semaphore(Semaphore &&) noexcept = default;
    Semaphore & operator=(Semaphore &&) noexcept = default;

    ~Semaphore() = default;

    explicit operator bool() const;

    bool IsValid() const;

    const vk::Semaphore & SemaphoreHandle() const;
    vk::Semaphore & SemaphoreHandle();

private:
    vk::UniqueSemaphore m_semaphore = {};
};

}  // namespace Gris::Graphics::Vulkan
