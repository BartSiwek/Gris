#include <gris/graphics/vulkan/semaphore.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Semaphore::Semaphore(Device * device)
    : DeviceResource(device)
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

const vk::Semaphore & Gris::Graphics::Vulkan::Semaphore::SemaphoreHandle() const
{
    return m_semaphore.get();
}

// -------------------------------------------------------------------------------------------------

vk::Semaphore & Gris::Graphics::Vulkan::Semaphore::SemaphoreHandle()
{
    return m_semaphore.get();
}
