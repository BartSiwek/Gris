#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Sampler : public DeviceResource
{
public:
    Sampler(Device * device, float minLod, float maxLod);

    const vk::Sampler & SamplerHandle() const;
    vk::Sampler & SamplerHandle();

private:
    vk::UniqueSampler m_sampler;
};

}  // namespace Gris::Graphics::Vulkan
