#include <gris/graphics/vulkan/texture.h>

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Texture::Texture() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Texture::Texture(const ParentObject<Device> & device,
                                         uint32_t width,
                                         uint32_t height,
                                         uint32_t mipLevels,
                                         vk::SampleCountFlagBits numSamples,
                                         vk::Format format,
                                         vk::ImageTiling tiling,
                                         const vk::ImageUsageFlags & usage,
                                         const vk::MemoryPropertyFlags & properties)
    : DeviceResource(device)
    , m_mipLevels(mipLevels)
{
    auto const imageInfo = vk::ImageCreateInfo{}
                               .setImageType(vk::ImageType::e2D)
                               .setFormat(format)
                               .setExtent(vk::Extent3D(width, height, 1))
                               .setMipLevels(m_mipLevels)
                               .setArrayLayers(1)
                               .setSamples(numSamples)
                               .setTiling(tiling)
                               .setUsage(usage)
                               .setSharingMode(vk::SharingMode::eExclusive)
                               .setQueueFamilyIndices({})
                               .setInitialLayout(vk::ImageLayout::eUndefined);

    auto createImageResult = DeviceHandle().createImage(imageInfo, nullptr, Dispatch());
    if (createImageResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating image", createImageResult);
    }

    m_image = std::move(createImageResult.value);

    auto allocationInfo = VmaAllocationCreateInfo{};
    allocationInfo.flags = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    allocationInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(properties);
    allocationInfo.preferredFlags = {};
    allocationInfo.memoryTypeBits = 0;
    allocationInfo.pool = {};
    allocationInfo.pUserData = nullptr;
    m_imageMemory = AllocatorHandle().AllocateMemory(m_image, allocationInfo);

    AllocatorHandle().Bind(m_image, m_imageMemory);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Texture::Texture(Texture && other) noexcept
    : DeviceResource(std::move(other))
    , m_image(std::exchange(other.m_image, {}))
    , m_imageMemory(std::exchange(other.m_imageMemory, {}))
    , m_mipLevels(std::exchange(other.m_mipLevels, 1))
{

}

// -------------------------------------------------------------------------------------------------    

Gris::Graphics::Vulkan::Texture & Gris::Graphics::Vulkan::Texture::operator=(Texture && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        DeviceResource::operator=(std::move(other));
        m_image = std::exchange(other.m_image, {});
        m_imageMemory = std::exchange(other.m_imageMemory, {});
        m_mipLevels = std::exchange(other.m_mipLevels, 1);
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Texture::~Texture()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Texture::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Texture::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_image);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Image & Gris::Graphics::Vulkan::Texture::ImageHandle() const
{
    return m_image;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Image & Gris::Graphics::Vulkan::Texture::ImageHandle()
{
    return m_image;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Texture::Reset()
{
    m_mipLevels = 1;

    if (m_imageMemory)
    {
        m_imageMemory.Reset();
    }

    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Texture::ReleaseResources()
{
    if (m_image)
    {
        DeviceHandle().destroyImage(m_image, nullptr, Dispatch());
        m_image = nullptr;
    }
}
