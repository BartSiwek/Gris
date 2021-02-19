#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Device;

class Semaphore : public DeviceResource
{
public:
    Semaphore();

    explicit Semaphore(const ParentObject<Device> & device);

    Semaphore(const Semaphore &) = delete;
    Semaphore & operator=(const Semaphore &) = delete;

    Semaphore(Semaphore && other) noexcept;
    Semaphore & operator=(Semaphore && other) noexcept;

    virtual ~Semaphore();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    const vk::Semaphore & SemaphoreHandle() const;
    vk::Semaphore & SemaphoreHandle();

private:
    void Reset();

    vk::Semaphore m_semaphore = {};
};

}  // namespace Gris::Graphics::Vulkan
