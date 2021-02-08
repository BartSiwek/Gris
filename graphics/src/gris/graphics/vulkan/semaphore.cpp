#include <gris/graphics/vulkan/semaphore.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore(std::shared_ptr<DeviceSharedData> sharedData)
    : DeviceResource(sharedData)
{
    auto const semaphoreInfo = vk::SemaphoreCreateInfo{};

    auto imageSemaphoreCreateResult = DeviceHandle().createSemaphoreUnique(semaphoreInfo, nullptr, Dispatch());
    if (imageSemaphoreCreateResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating frame image available semaphore", imageSemaphoreCreateResult);
    }

    m_semaphore = std::move(imageSemaphoreCreateResult.value);
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
    return m_semaphore.get();
}

// -------------------------------------------------------------------------------------------------

vk::Semaphore & Gris::Graphics::Vulkan::Semaphore::SemaphoreHandle()
{
    return m_semaphore.get();
}
