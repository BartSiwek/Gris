#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanSampler : public VulkanDeviceResource
{
public:
    VulkanSampler(VulkanDevice * device, float minLod, float maxLod);

    // TODO: Do this better
    const vk::Sampler & SamplerHandle() const;
    vk::Sampler & SamplerHandle();

private:
    vk::UniqueSampler m_sampler;
};

}  // namespace Gris::Graphics::Vulkan
