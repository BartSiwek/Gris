#include <gris/graphics/vulkan/semaphore.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore(const ParentObject<Device> & device)
    : DeviceResource(device)
{
    auto const semaphoreInfo = vk::SemaphoreCreateInfo{};

    auto imageSemaphoreCreateResult = DeviceHandle().createSemaphore(semaphoreInfo, nullptr, Dispatch());
    if (imageSemaphoreCreateResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating frame image available semaphore", imageSemaphoreCreateResult);
    }

    m_semaphore = std::move(imageSemaphoreCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore(Semaphore && other) noexcept
    : DeviceResource(std::move(other))
    , m_semaphore(std::exchange(other.m_semaphore, {}))
{

}

// -------------------------------------------------------------------------------------------------    
    
Gris::Graphics::Vulkan::Semaphore & Gris::Graphics::Vulkan::Semaphore::operator=(Semaphore && other) noexcept
{
    if (this != &other)
    {
        Reset();

        DeviceResource::operator=(std::move(other));
        m_semaphore = std::exchange(other.m_semaphore, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::~Semaphore()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Semaphore::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_semaphore);
}

// -------------------------------------------------------------------------------------------------

const vk::Semaphore & Gris::Graphics::Vulkan::Semaphore::SemaphoreHandle() const
{
    return m_semaphore;
}

// -------------------------------------------------------------------------------------------------

vk::Semaphore & Gris::Graphics::Vulkan::Semaphore::SemaphoreHandle()
{
    return m_semaphore;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Semaphore::Reset()
{
    if (m_semaphore)
    {
        DeviceHandle().destroySemaphore(m_semaphore, nullptr, Dispatch());
        m_semaphore = nullptr;
    }
}
