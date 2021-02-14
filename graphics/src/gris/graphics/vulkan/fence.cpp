#include <gris/graphics/vulkan/fence.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::Fence() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::Fence(const ParentObject<Device> & device, bool signaled)
    : DeviceResource(device)
{
    vk::FenceCreateFlags flags;
    if (signaled)
    {
        flags |= vk::FenceCreateFlagBits::eSignaled;
    }

    auto const fenceInfo = vk::FenceCreateInfo{}.setFlags(flags);

    auto fenceCreateResult = DeviceHandle().createFenceUnique(fenceInfo, nullptr, Dispatch());
    if (fenceCreateResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating frame fence", fenceCreateResult);
    }

    m_fence = std::move(fenceCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Fence::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_fence);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Fence & Gris::Graphics::Vulkan::Fence::FenceHandle() const
{
    return m_fence.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Fence & Gris::Graphics::Vulkan::Fence::FenceHandle()
{
    return m_fence.get();
}
