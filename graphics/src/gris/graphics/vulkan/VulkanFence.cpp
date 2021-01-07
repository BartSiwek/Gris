#include <gris/graphics/vulkan/VulkanFence.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanFence::VulkanFence(VulkanDevice * device, bool signaled)
    : VulkanDeviceResource(device)
{
    vk::FenceCreateFlags flags;
    if (signaled)
        flags |= vk::FenceCreateFlagBits::eSignaled;

    auto const fenceInfo = vk::FenceCreateInfo(flags);

    auto fenceCreateResult = DeviceHandle().createFenceUnique(fenceInfo);
    if (fenceCreateResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating frame fence", fenceCreateResult);

    m_fence = std::move(fenceCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Fence & Gris::Graphics::Vulkan::VulkanFence::FenceHandle() const
{
    return m_fence.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::Fence & Gris::Graphics::Vulkan::VulkanFence::FenceHandle()
{
    return m_fence.get();
}
