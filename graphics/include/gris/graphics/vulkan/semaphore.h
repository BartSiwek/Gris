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

    ~Semaphore() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Semaphore & SemaphoreHandle() const;
    [[nodiscard]] vk::Semaphore & SemaphoreHandle();

    void Reset();

private:
    void ReleaseResources();

    vk::Semaphore m_semaphore = {};
};

}  // namespace Gris::Graphics::Vulkan
