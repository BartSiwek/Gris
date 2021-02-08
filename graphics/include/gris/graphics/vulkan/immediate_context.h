#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Buffer;
class Texture;
class DeferredContext;
class Semaphore;
class Fence;

class ImmediateContext : public DeviceResource
{
public:
    ImmediateContext();

    explicit ImmediateContext(std::shared_ptr<DeviceSharedData> sharedData);

    ImmediateContext(const ImmediateContext &) = delete;
    ImmediateContext & operator=(const ImmediateContext &) = delete;

    ImmediateContext(ImmediateContext &&) noexcept = default;
    ImmediateContext & operator=(ImmediateContext &&) noexcept = default;

    ~ImmediateContext() = default;

    explicit operator bool() const;

    bool IsValid() const;

    void GenerateMipmaps(const Texture & texture, const vk::Format & imageFormat, int32_t texWidth, int32_t texHeight);
    void CopyBufferToImage(const Buffer & buffer, const Texture & texture, uint32_t width, uint32_t height);
    void TransitionImageLayout(const Texture & texture, const vk::ImageLayout & oldLayout, const vk::ImageLayout & newLayout);
    void CopyBuffer(const Buffer & srcBuffer, const Buffer & dstBuffer, vk::DeviceSize size);
    void Submit(DeferredContext * context, const std::vector<std::reference_wrapper<Semaphore>> & waitSemaphores, const std::vector<std::reference_wrapper<Semaphore>> & signalSemaphores, Fence & fence);

private:
    [[nodiscard]] vk::UniqueCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(vk::CommandBuffer & commandBuffer) const;

    vk::Queue m_graphicsQueue = {};
    vk::UniqueCommandPool m_commandPool = {};
};

}  // namespace Gris::Graphics::Vulkan