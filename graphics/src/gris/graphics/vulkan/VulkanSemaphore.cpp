#include <gris/graphics/vulkan/VulkanSemaphore.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanSemaphore::VulkanSemaphore(VulkanDevice* device) : VulkanDeviceResource(device)
{
    auto const semaphoreInfo = vk::SemaphoreCreateInfo();

    auto imageSemaphoreCreateResult = DeviceHandle().createSemaphoreUnique(semaphoreInfo);
    if (imageSemaphoreCreateResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating frame image available semaphore", imageSemaphoreCreateResult);

    m_semaphore = std::move(imageSemaphoreCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

const vk::Semaphore& VulkanSemaphore::SemaphoreHandle() const
{
    return m_semaphore.get();
}

// -------------------------------------------------------------------------------------------------

vk::Semaphore& VulkanSemaphore::SemaphoreHandle()
{
    return m_semaphore.get();
}
