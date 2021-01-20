#include <gris/graphics/vulkan/sampler.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::Sampler(Device * device, float minLod, float maxLod)
    : DeviceResource(device)
{
    auto const samplerInfo = vk::SamplerCreateInfo{}
                                      .setMinFilter(vk::Filter::eLinear)
                                      .setMagFilter(vk::Filter::eLinear)
                                      .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                                      .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                                      .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                                      .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                                      .setMipLodBias(0.0F)
                                      .setAnisotropyEnable(static_cast<vk::Bool32>(true))
                                      .setMaxAnisotropy(16.0F)
                                      .setCompareEnable(static_cast<vk::Bool32>(false))
                                      .setCompareOp(vk::CompareOp::eAlways)
                                      .setMinLod(minLod)
                                      .setMaxLod(maxLod)
                                      .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                                      .setUnnormalizedCoordinates(static_cast<vk::Bool32>(false));

    auto createSamplerResult = DeviceHandle().createSamplerUnique(samplerInfo);
    if (createSamplerResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating sampler", createSamplerResult);
    }

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
