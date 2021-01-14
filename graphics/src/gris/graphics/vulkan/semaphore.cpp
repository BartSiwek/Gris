#include <gris/graphics/vulkan/semaphore.h>

#include <gris/graphics/vulkan/engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanSemaphore::VulkanSemaphore(VulkanDevice * device)
    : VulkanDeviceResource(device)
{
    auto const semaphoreInfo = vk::SemaphoreCreateInfo();

    auto imageSemaphoreCreateResult = DeviceHandle().createSemaphoreUnique(semaphoreInfo);
    if (imageSemaphoreCreateResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating frame image available semaphore", imageSemaphoreCreateResult);

    m_semaphore = std::move(imageSemaphoreCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

const vk::Semaphore & Gris::Graphics::Vulkan::VulkanSemaphore::SemaphoreHandle() const
{
    return m_semaphore.get();
}

// -------------------------------------------------------------------------------------------------

vk::Semaphore & Gris::Graphics::Vulkan::VulkanSemaphore::SemaphoreHandle()
{
    return m_semaphore.get();
}
