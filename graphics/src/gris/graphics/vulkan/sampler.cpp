#include <gris/graphics/vulkan/sampler.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::Sampler(Device * device, float minLod, float maxLod)
    : DeviceResource(device)
{
    auto const samplerInfo = vk::SamplerCreateInfo({},
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
                                                   false);

    auto createSamplerResult = DeviceHandle().createSamplerUnique(samplerInfo);
    if (createSamplerResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating sampler", createSamplerResult);

    m_sampler = std::move(createSamplerResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Sampler & Gris::Graphics::Vulkan::Sampler::SamplerHandle() const
{
    return m_sampler.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Sampler & Gris::Graphics::Vulkan::Sampler::SamplerHandle()
{
    return m_sampler.get();
}
