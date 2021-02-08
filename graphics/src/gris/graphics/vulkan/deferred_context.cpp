#include <gris/graphics/vulkan/deferred_context.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/shader_resource_bindings.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeferredContext::DeferredContext() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeferredContext::DeferredContext(std::shared_ptr<Device *> device)
    : DeviceResource(device)
{
    auto const queueFamilies = ParentDevice().QueueFamilies();
    auto const graphicsQueueFamily = queueFamilies.graphicsFamily.value();

    auto const poolInfo = vk::CommandPoolCreateInfo{}.setQueueFamilyIndex(graphicsQueueFamily);

    auto createCommandPoolResult = DeviceHandle().createCommandPoolUnique(poolInfo, nullptr, Dispatch());
    if (createCommandPoolResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating command pool", createCommandPoolResult);
    }

    m_commandPool = std::move(createCommandPoolResult.value);

    auto const allocInfo = vk::CommandBufferAllocateInfo{}
                               .setCommandPool(m_commandPool.get())
                               .setLevel(vk::CommandBufferLevel::ePrimary)
                               .setCommandBufferCount(1);

    auto allocateCommandBuffersResult = DeviceHandle().allocateCommandBuffersUnique(allocInfo, Dispatch());
    if (allocateCommandBuffersResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error allocating command buffers", allocateCommandBuffersResult);
    }

    GRIS_ALWAYS_ASSERT(allocateCommandBuffersResult.value.size() == 1, "Number of allocated command buffers should be one");
    m_commandBuffer = std::move(allocateCommandBuffersResult.value.front());
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeferredContext::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::DeferredContext::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_commandPool) && static_cast<bool>(m_commandBuffer);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::CommandBuffer & Gris::Graphics::Vulkan::DeferredContext::CommandBufferHandle()
{
    return m_commandBuffer.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::Begin()
{
    auto const beginInfo = vk::CommandBufferBeginInfo{};

    auto const beginResult = m_commandBuffer->begin(beginInfo, Dispatch());
    if (beginResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error beginning command buffer", beginResult);
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BeginRenderPass(const RenderPass & renderPass, const Framebuffer & framebuffer, const vk::Extent2D & extent)
{
    std::array<vk::ClearValue, 2> clearValues = {
        vk::ClearColorValue(std::array{ 0.0F, 0.0F, 0.0F, 1.0F }),
        vk::ClearDepthStencilValue(1.0F, 0)
    };

    auto const renderPassInfo = vk::RenderPassBeginInfo{}
                                    .setRenderPass(renderPass.RenderPassHandle())
                                    .setFramebuffer(framebuffer.FramebufferHandle())
                                    .setRenderArea(vk::Rect2D({ 0, 0 }, extent))
                                    .setClearValues(clearValues);

    m_commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindPipeline(const PipelineStateObject & pso)
{
    m_commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pso.GraphicsPipelineHandle(), Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindVertexBuffer(const BufferView & bufferView)
{
    std::array vertexBuffers = { bufferView.BufferHandle() };
    std::array offsets = { static_cast<vk::DeviceSize>(bufferView.Offset()) };
    m_commandBuffer->bindVertexBuffers(0, vertexBuffers, offsets, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindIndexBuffer(const BufferView & bufferView)
{
    m_commandBuffer->bindIndexBuffer(bufferView.BufferHandle(), bufferView.Offset(), vk::IndexType::eUint32, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindDescriptorSet(const PipelineStateObject & pso, const ShaderResourceBindings & srb)
{
    std::array descriptorSets = { srb.DescriptorSetHandle() };
    m_commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pso.PipelineLayoutHandle(), 0, descriptorSets, {}, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::DrawIndexed(uint32_t indexCount)
{
    m_commandBuffer->drawIndexed(indexCount, 1, 0, 0, 0, Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::EndRenderPass()
{
    m_commandBuffer->endRenderPass(Dispatch());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::End()
{
    auto const endResult = m_commandBuffer->end(Dispatch());
    if (endResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error ending command buffer", endResult);
    }
}
