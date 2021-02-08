#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Sampler : public DeviceResource
{
public:
    Sampler();

    Sampler(std::shared_ptr<Device *> device, float minLod, float maxLod);

    Sampler(const Sampler &) = delete;
    Sampler & operator=(const Sampler &) = delete;

    Sampler(Sampler &&) noexcept = default;
    Sampler & operator=(Sampler &&) noexcept = default;

    ~Sampler() = default;

    explicit operator bool() const;

    bool IsValid() const;

    const vk::Sampler & SamplerHandle() const;
    vk::Sampler & SamplerHandle();

private:
    vk::UniqueSampler m_sampler = {};
};

}  // namespace Gris::Graphics::Vulkan
