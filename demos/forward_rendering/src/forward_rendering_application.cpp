#include "forward_rendering_application.h"

#include <gris/graphics/image.h>
#include <gris/graphics/loaders/assimp_mesh_loader.h>
#include <gris/graphics/loaders/stb_image_loader.h>
#include <gris/graphics/mesh.h>

#include <gris/graphics/vulkan/buffer.h>
#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/deferred_context.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/glfw/window.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/input_layout.h>
#include <gris/graphics/vulkan/physical_device_factory.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/shader_resource_bindings.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>

#include <gris/graphics/glfw/instance.h>

#include <gris/directory_registry.h>
#include <gris/engine_exception.h>
#include <gris/log.h>
#include <gris/utils.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cstdint>
#include <vector>

// -------------------------------------------------------------------------------------------------

constexpr static uint32_t INITIAL_WINDOW_WIDTH = 800;
constexpr static uint32_t INITIAL_WINDOW_HEIGHT = 600;

const char * const MODEL_PATH = "sponza.dae";
const char * const TEXTURE_PATH = "viking_room.png";
const char * const VERTEX_SHADER_PATH = "vertex.spv";
const char * const FRAGMENT_SHADER_PATH = "fragment.spv";

constexpr static int MAX_FRAMES_IN_FLIGHT = 3;

constexpr static size_t GlslMatrixAlignment = 16;

// -------------------------------------------------------------------------------------------------

struct UniformBufferObject
{
    alignas(GlslMatrixAlignment) glm::mat4 model;
    alignas(GlslMatrixAlignment) glm::mat4 view;
    alignas(GlslMatrixAlignment) glm::mat4 proj;
};

// -------------------------------------------------------------------------------------------------

namespace
{

inline glm::vec2 GetNormalizedScreenCoordinates(float width, float height, float x, float y)
{
    auto const normalizedPoint = glm::vec2((2 * x) / width - 1, 1 - (2 * y) / height);
    return glm::clamp(normalizedPoint, glm::vec2(-1, -1), glm::vec2(1, 1));
}

}  // namespace

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::Run()
{
    InitWindow();
    CreateVulkanObjects();
    MainLoop();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format ForwardRenderingApplication::FindDepthFormat() const
{
    return m_device.FindSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::WindowResized(uint32_t /* width */, uint32_t /* height */)
{
    m_framebufferResized = true;
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::MouseButtonEvent(Gris::Graphics::MouseButton button, Gris::Graphics::MouseButtonAction action, float x, float y)
{
    if (action == Gris::Graphics::MouseButtonAction::Down)
    {
        if (button == Gris::Graphics::MouseButton::Right)
        {
            auto const swapChainExtent = m_swapChain.Extent();
            m_camera.SetDesiredState(Gris::Graphics::Cameras::TrackballCameraOperation::Panning);
            m_camera.SetCurrentPoint(GetNormalizedScreenCoordinates(static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), x, y));
        }
        else if (button == Gris::Graphics::MouseButton::Left)
        {
            auto const swapChainExtent = m_swapChain.Extent();
            m_camera.SetDesiredState(Gris::Graphics::Cameras::TrackballCameraOperation::Rotating);
            m_camera.SetCurrentPoint(GetNormalizedScreenCoordinates(static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), x, y));
        }
        else if (button == Gris::Graphics::MouseButton::Middle)
        {
            auto const swapChainExtent = m_swapChain.Extent();
            m_camera.SetDesiredState(Gris::Graphics::Cameras::TrackballCameraOperation::Zooming);
            m_camera.SetCurrentPoint(GetNormalizedScreenCoordinates(static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), x, y));
        }
    }
    else
    {
        m_camera.SetDesiredState(Gris::Graphics::Cameras::TrackballCameraOperation::None);
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::MouseMoveEvent(float x, float y)
{
    auto const swapChainExtent = m_swapChain.Extent();
    m_camera.SetCurrentPoint(GetNormalizedScreenCoordinates(static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), x, y));
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::MouseWheelEvent(float /* x */, float /* y */, float delta)
{
    constexpr static float ZOOM_IN_FACTOR = 1.1F;
    constexpr static float ZOOM_OUT_FACTOR = 0.9F;

    if (delta > 0)
    {
        m_lens.SetZoomFactor(ZOOM_IN_FACTOR * m_lens.GetZoomFactor());
    }
    else
    {
        m_lens.SetZoomFactor(ZOOM_OUT_FACTOR * m_lens.GetZoomFactor());
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::InitWindow()
{
    m_window = Gris::Graphics::Vulkan::Glfw::Window(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Forward rendering demo");
    m_window.AddObserver(this);
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateVulkanObjects()
{
    if (!m_device)
    {
        CreateDevice();
    }
    else
    {
        WaitForDeviceToBeIdle();
    }

    CreateSwapChain();
    CreateRenderPass();

    if (!m_pso)
    {
        CreatePipelineStateObject();
    }

    CreateFramebuffers();
    CreateShaderResourceBindingsPools();

    if (m_meshes.empty())
    {
        LoadScene();
    }

    CreateUniformBuffersAndBindings();
    CreateCommandBuffers();
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::LoadScene()
{
    CreateCamera();
    CreateMesh();
    CreateMeshTexture();
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::MainLoop()
{
    while (!m_window.ShouldClose())
    {
        Gris::Graphics::Glfw::Instance::PollEvents();
        DrawFrame();
    }

    m_device.WaitIdle();
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateDevice()
{
    m_device = Gris::Graphics::Vulkan::Device(FindSuitablePhysicalDevice(m_window));
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::WaitForDeviceToBeIdle() const
{
    m_device.WaitIdle();
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateSwapChain()
{
    m_swapChain = m_device.CreateSwapChain(m_window, m_window.Width(), m_window.Height(), MAX_FRAMES_IN_FLIGHT, std::move(m_swapChain));
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateRenderPass()
{
    m_renderPass = m_device.CreateRenderPass(m_swapChain.Format(), FindDepthFormat());
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateCamera()
{
    constexpr static glm::vec3 CAMERA_DEFAULT_LOCATION = glm::vec3(0.0F, 5.0F, 0.0F);
    constexpr static float CAMERA_DEFAULT_RADIUS = 5.0F;
    constexpr static float CAMERA_DEFAULT_PANNING_SPEED = 10.0F;

    constexpr static float LENS_DEFAULT_NEAR_PLANE = 1.0F;
    constexpr static float LENS_DEFAULT_FAR_PLANE = 1000.0F;
    constexpr static float LENS_DEFAULT_FOV = glm::radians(90.0F);

    m_camera.SetLocation(CAMERA_DEFAULT_LOCATION);
    m_camera.SetRadius(CAMERA_DEFAULT_RADIUS);
    m_camera.SetPanningSpeed(CAMERA_DEFAULT_PANNING_SPEED);

    auto const swapChainExtent = m_swapChain.Extent();
    m_lens.SetFrustum(LENS_DEFAULT_NEAR_PLANE, LENS_DEFAULT_FAR_PLANE, static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), LENS_DEFAULT_FOV);
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateMesh()
{
    auto modelPath = Gris::DirectoryRegistry::TryResolvePath(MODEL_PATH);
    if (!modelPath)
    {
        throw Gris::EngineException("Error resolving model path - file not found", MODEL_PATH);
    }

    m_meshes = Gris::Graphics::Loaders::AssimpMeshLoader::Load(*modelPath);

    ///

    for (auto const & mesh : m_meshes)
    {
        auto const vertexBufferSize = sizeof(mesh.Vertices[0]) * mesh.Vertices.size();

        auto vertexStagingBuffer = m_device.CreateBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        vertexStagingBuffer.SetData(mesh.Vertices.data(), static_cast<size_t>(vertexBufferSize));

        auto & vertexBuffer = m_vertexBuffers.emplace_back(m_device.CreateBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device.Context().CopyBuffer(vertexStagingBuffer, vertexBuffer, vertexBufferSize);

        m_vertexBufferViews.emplace_back(Gris::Graphics::Vulkan::BufferView(vertexBuffer, 0, static_cast<uint32_t>(vertexBufferSize)));
    }

    ///

    for (auto const & mesh : m_meshes)
    {
        auto const indexBufferSize = sizeof(mesh.Indices[0]) * mesh.Indices.size();

        auto indexStagingBuffer = m_device.CreateBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        indexStagingBuffer.SetData(mesh.Indices.data(), static_cast<size_t>(indexBufferSize));

        auto & indexBuffer = m_indexBuffers.emplace_back(m_device.CreateBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device.Context().CopyBuffer(indexStagingBuffer, indexBuffer, indexBufferSize);

        m_indexBufferViews.emplace_back(Gris::Graphics::Vulkan::BufferView(indexBuffer, 0, static_cast<uint32_t>(indexBufferSize)));
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateMeshTexture()
{
    auto const texturePath = Gris::DirectoryRegistry::TryResolvePath(TEXTURE_PATH);
    if (!texturePath)
    {
        throw Gris::EngineException("Failed to resolve texture image path", TEXTURE_PATH);
    }

    auto image = Gris::Graphics::Loaders::StbImageLoader::Load(*texturePath);

    ///

    auto const mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(image.Width, image.Height)))) + 1;
    m_meshTextureImage = m_device.CreateTexture(image.Width, image.Height, mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

    ///

    m_device.Context().TransitionImageLayout(m_meshTextureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    auto stagingBuffer = m_device.CreateBuffer(image.PixelData.size(), vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    stagingBuffer.SetData(image.PixelData.data(), image.PixelData.size());
    m_device.Context().CopyBufferToImage(stagingBuffer, m_meshTextureImage, image.Width, image.Height);

    ///

    m_device.Context().GenerateMipmaps(m_meshTextureImage, vk::Format::eR8G8B8A8Srgb, image.Width, image.Height);

    ///

    m_meshTextureImageView = m_device.CreateTextureView(m_meshTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, m_meshTextureImage.MipLevels());
    m_meshTextureSampler = m_device.CreateSampler(0.0F, static_cast<float>(m_meshTextureImage.MipLevels()));
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreatePipelineStateObject()
{
    if (!m_vertexShader)
    {
        auto const vertexShaderPath = Gris::DirectoryRegistry::TryResolvePath(VERTEX_SHADER_PATH);
        if (!vertexShaderPath)
        {
            throw Gris::EngineException("Error resolving vertex shader path", VERTEX_SHADER_PATH);
        }

        m_vertexShader = m_device.CreateShader(Gris::ReadFile<uint32_t>(*vertexShaderPath), "main");
    }

    ///

    if (!m_fragmentShader)
    {
        auto const fragmentShaderPath = Gris::DirectoryRegistry::TryResolvePath(FRAGMENT_SHADER_PATH);
        if (!fragmentShaderPath)
        {
            throw Gris::EngineException("Error resolving fragment shader path", FRAGMENT_SHADER_PATH);
        }

        m_fragmentShader = m_device.CreateShader(Gris::ReadFile<uint32_t>(*fragmentShaderPath), "main");
    }

    ///

    if (!m_resourceLayouts[GLOBAL_DESCRIPTOR_SET_INDEX])
    {
        auto const resourceLayouts = std::array{
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "ubo",
                0,
                Gris::Graphics::Backend::ShaderResourceType::UniformBuffer,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Vertex,
            },
        };
        auto const bindingsLayout = Gris::Graphics::Backend::ShaderResourceBindingsLayout{ resourceLayouts };
        m_resourceLayouts[GLOBAL_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindingsLayout(bindingsLayout);
    }

    if (!m_resourceLayouts[PER_MATERIAL_DESCRIPTOR_SET_INDEX])
    {
        auto const resourceLayouts = std::array{
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "texSampler",
                0,
                Gris::Graphics::Backend::ShaderResourceType::CombinedImageSampler,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Fragment,
            },
        };
        auto const bindingsLayout = Gris::Graphics::Backend::ShaderResourceBindingsLayout{ resourceLayouts };
        m_resourceLayouts[PER_MATERIAL_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindingsLayout(bindingsLayout);
    }

    if (!m_resourceLayouts[PER_DRAW_DESCRIPTOR_SET_INDEX])
    {
        auto const resourceLayouts = std::array<Gris::Graphics::Backend::ShaderResourceBindingLayout, 0>{};
        auto const bindingsLayout = Gris::Graphics::Backend::ShaderResourceBindingsLayout{ resourceLayouts };
        m_resourceLayouts[PER_DRAW_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindingsLayout(bindingsLayout);
    }

    ///

    Gris::Graphics::Vulkan::InputLayout layout;
    layout.AddBinding(0, sizeof(Gris::Graphics::Vertex), vk::VertexInputRate::eVertex);
    layout.AddAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Gris::Graphics::Vertex, Position));
    layout.AddAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Gris::Graphics::Vertex, Color));
    layout.AddAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Gris::Graphics::Vertex, TextureCoords));

    m_pso = m_device.CreatePipelineStateObject({}, {}, m_renderPass, layout, m_resourceLayouts, m_vertexShader, m_fragmentShader);
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateFramebuffers()
{
    auto const swapChainExtent = m_swapChain.Extent();
    auto const swapChainFormat = m_swapChain.Format();
    auto const depthFormat = FindDepthFormat();

    m_colorImage = m_device.CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device.MsaaSamples(), swapChainFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_colorImageView = m_device.CreateTextureView(m_colorImage, swapChainFormat, vk::ImageAspectFlagBits::eColor, 1);

    m_depthImage = m_device.CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device.MsaaSamples(), depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_depthImageView = m_device.CreateTextureView(m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);

    m_swapChainFramebuffers.resize(m_swapChain.ImageCount());
    for (size_t i = 0; i < m_swapChain.ImageCount(); i++)
    {
        m_swapChainFramebuffers[i] = m_device.CreateFramebuffer(m_colorImageView, m_depthImageView, m_swapChain.ImageView(i), m_renderPass, swapChainExtent.width, swapChainExtent.height);
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateShaderResourceBindingsPools()
{
    auto sizes = Gris::Graphics::Backend::ShaderResourceBindingsPoolSizes{};
    sizes.ShaderResourceBindingsCount = m_swapChain.VirtualFrameCount();
    sizes.CombinedImageSamplerCount = m_swapChain.VirtualFrameCount();
    sizes.UniformBufferCount = m_swapChain.VirtualFrameCount();

    if (!m_shaderResourceBindingsPools)
    {
        m_device.RegisterShaderResourceBindingsPoolCategory(m_shaderResourceBindingsPoolCategory, sizes);
        m_shaderResourceBindingsPools = m_device.CreateShaderResourceBindingsPoolCollection();
    }
    else
    {
        m_device.UpdateShaderResourceBindingsPoolCategory(m_shaderResourceBindingsPoolCategory, sizes);
        m_shaderResourceBindingsPools.Clear();
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateUniformBuffersAndBindings()
{
    m_uniformBuffers.resize(m_swapChain.VirtualFrameCount());
    m_uniformBufferViews.resize(m_swapChain.VirtualFrameCount());
    m_shaderResourceBindings.resize(m_swapChain.VirtualFrameCount());
    for (size_t i = 0; i < m_swapChain.VirtualFrameCount(); i++)
    {
        m_uniformBuffers[i] = m_device.CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        m_uniformBufferViews[i] = Gris::Graphics::Vulkan::BufferView(m_uniformBuffers[i], 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));

        m_shaderResourceBindings[i][GLOBAL_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindings(m_resourceLayouts[GLOBAL_DESCRIPTOR_SET_INDEX]);
        m_shaderResourceBindings[i][PER_MATERIAL_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindings(m_resourceLayouts[PER_MATERIAL_DESCRIPTOR_SET_INDEX]);
        m_shaderResourceBindings[i][PER_DRAW_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindings(m_resourceLayouts[PER_DRAW_DESCRIPTOR_SET_INDEX]);

        m_shaderResourceBindings[i][GLOBAL_DESCRIPTOR_SET_INDEX].SetUniformBuffer("ubo", m_uniformBufferViews[i]);

        m_shaderResourceBindings[i][PER_MATERIAL_DESCRIPTOR_SET_INDEX].SetCombinedSamplerAndImageView("texSampler", m_meshTextureSampler, m_meshTextureImageView);

        m_shaderResourceBindings[i][GLOBAL_DESCRIPTOR_SET_INDEX].PrepareBindings(m_shaderResourceBindingsPoolCategory, &m_shaderResourceBindingsPools);
        m_shaderResourceBindings[i][PER_MATERIAL_DESCRIPTOR_SET_INDEX].PrepareBindings(m_shaderResourceBindingsPoolCategory, &m_shaderResourceBindingsPools);
        m_shaderResourceBindings[i][PER_DRAW_DESCRIPTOR_SET_INDEX].PrepareBindings(m_shaderResourceBindingsPoolCategory, &m_shaderResourceBindingsPools);
    }
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::CreateCommandBuffers()
{
    m_commandBuffers.resize(m_swapChain.VirtualFrameCount());
    for (uint32_t i = 0; i < m_swapChain.VirtualFrameCount(); i++)
        m_commandBuffers[i] = m_device.CreateDeferredContext(true);
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::UpdateUniformBuffer(uint32_t currentVirtualFrameIndex)
{
    auto const swapChainExtent = m_swapChain.Extent();
    auto const aspectRatio = static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);

    m_lens.UpdateMatrices(aspectRatio);
    m_camera.UpdateMatrices();

    UniformBufferObject ubo = {
        glm::mat4(1.0F),
        m_camera.GetViewMatrix(),
        m_lens.GetProjectionMatrix(),
    };
    ubo.proj[1][1] *= -1;

    m_uniformBuffers[currentVirtualFrameIndex].SetData(&ubo, sizeof(ubo));
}

// -------------------------------------------------------------------------------------------------

void ForwardRenderingApplication::DrawFrame()
{
    auto const nextImageResult = m_swapChain.NextImage();
    if (!nextImageResult)
    {
        CreateVulkanObjects();
        return;
    }

    auto const swapChainExtent = m_swapChain.Extent();

    UpdateUniformBuffer(nextImageResult->VirtualFrameIndex);

    m_commandBuffers[nextImageResult->VirtualFrameIndex].ResetContext(false);

    m_commandBuffers[nextImageResult->VirtualFrameIndex].Begin(true);
    m_commandBuffers[nextImageResult->VirtualFrameIndex].BeginRenderPass(m_renderPass, m_swapChainFramebuffers[nextImageResult->SwapChainImageIndex], m_swapChain.Extent());
    m_commandBuffers[nextImageResult->VirtualFrameIndex].BindPipeline(m_pso);
    m_commandBuffers[nextImageResult->VirtualFrameIndex].SetViewport(swapChainExtent.width, swapChainExtent.height);
    m_commandBuffers[nextImageResult->VirtualFrameIndex].SetScissor(swapChainExtent.width, swapChainExtent.height);
    m_commandBuffers[nextImageResult->VirtualFrameIndex].BindDescriptorSet(m_pso, 0, m_shaderResourceBindings[nextImageResult->VirtualFrameIndex]);

    for (size_t meshIndex = 0; meshIndex < m_meshes.size(); ++meshIndex)
    {
        m_commandBuffers[nextImageResult->VirtualFrameIndex].BindVertexBuffer(m_vertexBufferViews[meshIndex]);
        m_commandBuffers[nextImageResult->VirtualFrameIndex].BindIndexBuffer(m_indexBufferViews[meshIndex]);
        m_commandBuffers[nextImageResult->VirtualFrameIndex].DrawIndexed(static_cast<uint32_t>(m_meshes[meshIndex].Indices.size()));
    }

    m_commandBuffers[nextImageResult->VirtualFrameIndex].EndRenderPass();
    m_commandBuffers[nextImageResult->VirtualFrameIndex].End();

    auto const waitSemaphores = std::vector{ std::ref(m_swapChain.ImageAvailableSemaphore(*nextImageResult)) };
    auto const signalSemaphores = std::vector{ std::ref(m_swapChain.RenderingFinishedSemaphore(*nextImageResult)) };
    m_device.Context().Submit(&m_commandBuffers[nextImageResult->VirtualFrameIndex], waitSemaphores, signalSemaphores, m_swapChain.RenderingFinishedFence(*nextImageResult));

    auto const presentResult = m_swapChain.Present(*nextImageResult);
    if (!presentResult || m_framebufferResized)
    {
        CreateVulkanObjects();
        m_framebufferResized = false;
    }
}
