#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Sampler : public DeviceResource
{
public:
    Sampler();

    Sampler(const ParentObject<Device> & device, float minLod, float maxLod);

    Sampler(const Sampler &) = delete;
    Sampler & operator=(const Sampler &) = delete;

    Sampler(Sampler && other) noexcept;
    Sampler & operator=(Sampler && other) noexcept;

    virtual ~Sampler();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    const vk::Sampler & SamplerHandle() const;
    vk::Sampler & SamplerHandle();

    void Reset();

private:
    void ReleaseResources();

    vk::Sampler m_sampler = {};
};

}  // namespace Gris::Graphics::Vulkan
