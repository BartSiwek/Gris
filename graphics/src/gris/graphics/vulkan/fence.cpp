﻿#include <gris/graphics/vulkan/fence.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::Fence(Device * device, bool signaled)
    : DeviceResource(device)
{
    vk::FenceCreateFlags flags;
    if (signaled)
    {
        flags |= vk::FenceCreateFlagBits::eSignaled;
    }

    auto const fenceInfo = vk::FenceCreateInfo{}.setFlags(flags);

    auto fenceCreateResult = DeviceHandle().createFenceUnique(fenceInfo);
    if (fenceCreateResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating frame fence", fenceCreateResult);
    }

    m_fence = std::move(fenceCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Fence & Gris::Graphics::Vulkan::Fence::FenceHandle() const
{
    return m_fence.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::Fence & Gris::Graphics::Vulkan::Fence::FenceHandle()
{
    return m_fence.get();
}
