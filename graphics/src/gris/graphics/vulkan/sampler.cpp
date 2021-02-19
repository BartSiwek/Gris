#include <gris/graphics/vulkan/sampler.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::Sampler() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::Sampler(const ParentObject<Device> & device, float minLod, float maxLod)
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

    auto createSamplerResult = DeviceHandle().createSampler(samplerInfo, nullptr, Dispatch());
    if (createSamplerResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating sampler", createSamplerResult);
    }

    m_sampler = std::move(createSamplerResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::Sampler(Sampler && other) noexcept
    : DeviceResource(std::move(other))
    , m_sampler(std::exchange(other.m_sampler, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler & Gris::Graphics::Vulkan::Sampler::operator=(Sampler && other) noexcept
{
    if (this != &other)
    {
        Reset();

        DeviceResource::operator=(std::move(other));
        m_sampler = std::exchange(other.m_sampler, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::~Sampler()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Sampler::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Sampler::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_sampler);
}

// -------------------------------------------------------------------------------------------------

const vk::Sampler & Gris::Graphics::Vulkan::Sampler::SamplerHandle() const
{
    return m_sampler;
}

// -------------------------------------------------------------------------------------------------

vk::Sampler & Gris::Graphics::Vulkan::Sampler::SamplerHandle()
{
    return m_sampler;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Sampler::Reset()
{
    if (m_sampler)
    {
        DeviceHandle().destroySampler(m_sampler, nullptr, Dispatch());
        m_sampler = nullptr;
    }
}
