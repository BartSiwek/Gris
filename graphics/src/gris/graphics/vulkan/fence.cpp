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

    auto fenceCreateResult = DeviceHandle().createFence(fenceInfo, nullptr, Dispatch());
    if (fenceCreateResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating frame fence", fenceCreateResult);
    }

    m_fence = std::move(fenceCreateResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::Fence(Fence && other) noexcept
    : DeviceResource(std::move(other))
    , m_fence(std::exchange(other.m_fence, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence & Gris::Graphics::Vulkan::Fence::operator=(Fence && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        m_fence = std::exchange(other.m_fence, {});
        DeviceResource::operator=(std::move(other));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Fence::~Fence()
{
    ReleaseResources();
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
    return m_fence;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Fence & Gris::Graphics::Vulkan::Fence::FenceHandle()
{
    return m_fence;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Fence::Reset()
{
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Fence::ReleaseResources()
{
    if (m_fence)
    {
        DeviceHandle().destroyFence(m_fence, nullptr, Dispatch());
        m_fence = nullptr;
    }
}