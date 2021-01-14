#include <gris/graphics/vulkan/deferred_context.h>

#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/shader_resource_binding.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeferredContext::DeferredContext(Device * device)
    : DeviceResource(device)
{
    auto const queueFamilies = ParentDevice().QueueFamilies();
    auto const graphicsQueueFamily = queueFamilies.graphicsFamily.value();

    auto const poolInfo = vk::CommandPoolCreateInfo({}, graphicsQueueFamily);

    auto createCommandPoolResult = DeviceHandle().createCommandPoolUnique(poolInfo);
    if (createCommandPoolResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating command pool", createCommandPoolResult);

    m_commandPool = std::move(createCommandPoolResult.value);

    auto const allocInfo = vk::CommandBufferAllocateInfo(m_commandPool.get(), vk::CommandBufferLevel::ePrimary, 1);
    auto allocateCommandBuffersResult = DeviceHandle().allocateCommandBuffersUnique(allocInfo);
    if (allocateCommandBuffersResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating command buffers", allocateCommandBuffersResult);

    m_commandBuffers = std::move(allocateCommandBuffersResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::CommandBuffer & Gris::Graphics::Vulkan::DeferredContext::CommandBufferHandle()
{
    return m_commandBuffers[m_frameIndex].get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::Begin()
{
    auto const beginInfo = vk::CommandBufferBeginInfo();

    auto const beginResult = m_commandBuffers[m_frameIndex]->begin(beginInfo);
    if (beginResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error beginning command buffer", beginResult);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BeginRenderPass(const RenderPass & renderPass, const Framebuffer & framebuffer, const vk::Extent2D & extent)
{
    std::array<vk::ClearValue, 2> clearValues = {
        vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f }),
        vk::ClearDepthStencilValue(1.0f, 0)
    };

    auto const renderPassInfo = vk::RenderPassBeginInfo(renderPass.RenderPassHandle(), framebuffer.FramebufferHandle(), vk::Rect2D({ 0, 0 }, extent), clearValues);

    m_commandBuffers[m_frameIndex]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindPipeline(const PipelineStateObject & pso)
{
    m_commandBuffers[m_frameIndex]->bindPipeline(vk::PipelineBindPoint::eGraphics, pso.GraphicsPipelineHandle());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindVertexBuffer(const BufferView & bufferView)
{
    std::array vertexBuffers = { bufferView.BufferHandle() };
    std::array offsets = { static_cast<vk::DeviceSize>(bufferView.Offset()) };
    m_commandBuffers[m_frameIndex]->bindVertexBuffers(0, vertexBuffers, offsets);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindIndexBuffer(const BufferView & bufferView)
{
    m_commandBuffers[m_frameIndex]->bindIndexBuffer(bufferView.BufferHandle(), bufferView.Offset(), vk::IndexType::eUint32);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::BindDescriptorSet(const PipelineStateObject & pso, const ShaderResourceBinding & srb)
{
    std::array descriptorSets = { srb.DescriptorSetHandle(m_frameIndex) };
    m_commandBuffers[m_frameIndex]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pso.PipelineLayoutHandle(), 0, descriptorSets, {});
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::DrawIndexed(uint32_t indexCount)
{
    m_commandBuffers[m_frameIndex]->drawIndexed(indexCount, 1, 0, 0, 0);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::EndRenderPass()
{
    m_commandBuffers[m_frameIndex]->endRenderPass();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::DeferredContext::End()
{
    auto const endResult = m_commandBuffers[m_frameIndex]->end();
    if (endResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error ending command buffer", endResult);
}
