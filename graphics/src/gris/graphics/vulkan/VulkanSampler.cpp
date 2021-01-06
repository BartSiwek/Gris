#include <gris/graphics/vulkan/VulkanSampler.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanSampler::VulkanSampler(VulkanDevice* device, float minLod, float maxLod) : VulkanDeviceResource(device)
{
    auto const samplerInfo = vk::SamplerCreateInfo(
        {},
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        0.0f,
        true,
        16.0f,
        false,
        vk::CompareOp::eAlways,
        minLod,
        maxLod,
        vk::BorderColor::eIntOpaqueBlack,
        false
    );

    auto createSamplerResult = DeviceHandle().createSamplerUnique(samplerInfo);
    if (createSamplerResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating sampler", createSamplerResult);

    m_sampler = std::move(createSamplerResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Sampler& VulkanSampler::SamplerHandle() const
{
    return m_sampler.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Sampler& VulkanSampler::SamplerHandle()
{
    return m_sampler.get();
}
