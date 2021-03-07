#include "hello_triangle_application.h"

#include <gris/graphics/image.h>
#include <gris/graphics/loaders/stb_image_loader.h>
#include <gris/graphics/loaders/tinlyobjloader_mesh_loader.h>
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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <vector>

// -------------------------------------------------------------------------------------------------

constexpr static uint32_t INITIAL_WINDOW_WIDTH = 800;
constexpr static uint32_t INITIAL_WINDOW_HEIGHT = 600;

const char * const MODEL_PATH = "viking_room.obj";
const char * const TEXTURE_PATH = "viking_room.png";
const char * const VERTEX_SHADER_PATH = "vert.spv";
const char * const FRAGMENT_SHADER_PATH = "frag.spv";

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

void HelloTriangleApplication::Run()
{
    InitWindow();
    CreateVulkanObjects();
    MainLoop();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format HelloTriangleApplication::FindDepthFormat() const
{
    return m_device.FindSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::WindowResized(uint32_t /* width */, uint32_t /* height */)
{
    m_framebufferResized = true;
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::MouseButtonEvent(Gris::Graphics::MouseButton /* button */, Gris::Graphics::MouseButtonAction /* action */, float /* x */, float /* y */)
{
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::MouseMoveEvent(float /* x */, float /* y */)
{
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::MouseWheelEvent(float /* x */, float /* y */, float /* delta */)
{
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::InitWindow()
{
    m_window = Gris::Graphics::Vulkan::Glfw::Window(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Vulkan tutorial");
    m_window.AddObserver(this);
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateVulkanObjects()
{
    if (!m_device)
    {
        CreateDevice();
        LoadScene();
    }
    else
    {
        WaitForDeviceToBeIdle();
    }

    CreateSwapChain();
    CreateRenderPass();
    CreatePipelineStateObject();
    CreateFramebuffers();
    CreateShaderResourceBindingsPools();
    CreateUniformBuffersAndBindings();
    CreateCommandBuffers();
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::LoadScene()
{
    CreateMesh();
    CreateMeshTexture();
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::MainLoop()
{
    while (!m_window.ShouldClose())
    {
        Gris::Graphics::Glfw::Instance::PollEvents();
        DrawFrame();
    }

    m_device.WaitIdle();
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateDevice()
{
    m_device = Gris::Graphics::Vulkan::Device(Gris::Graphics::Vulkan::FindSuitablePhysicalDevice(m_window));
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::WaitForDeviceToBeIdle()
{
    m_device.WaitIdle();
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateSwapChain()
{
    m_swapChain = m_device.CreateSwapChain(m_window, m_window.Width(), m_window.Height(), MAX_FRAMES_IN_FLIGHT, std::move(m_swapChain));
    m_virtualFrames.resize(MAX_FRAMES_IN_FLIGHT);
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateRenderPass()
{
    m_renderPass = m_device.CreateRenderPass(m_swapChain.Format(), FindDepthFormat());
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateMesh()
{
    auto modelPath = Gris::DirectoryRegistry::TryResolvePath(MODEL_PATH);
    if (!modelPath)
    {
        throw Gris::EngineException("Error resolving model path - file not found", MODEL_PATH);
    }

    auto meshes = Gris::Graphics::Loaders::TinyObjLoaderMeshLoader::Load(*modelPath);
    if (meshes.size() != 1)
    {
        throw Gris::EngineException("Error loading sample mesh - expected a single mesh");
    }

    m_mesh = meshes.front();

    ///

    auto const vertexBufferSize = sizeof(m_mesh.Vertices[0]) * m_mesh.Vertices.size();

    auto vertexStagingBuffer = m_device.CreateBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    vertexStagingBuffer.SetData(m_mesh.Vertices.data(), static_cast<size_t>(vertexBufferSize));

    m_vertexBuffer = m_device.CreateBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_device.Context().CopyBuffer(vertexStagingBuffer, m_vertexBuffer, vertexBufferSize);

    m_vertexBufferView = Gris::Graphics::Vulkan::BufferView(m_vertexBuffer, 0, static_cast<uint32_t>(vertexBufferSize));

    ///

    auto const indexBufferSize = sizeof(m_mesh.Indices[0]) * m_mesh.Indices.size();

    auto indexStagingBuffer = m_device.CreateBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    indexStagingBuffer.SetData(m_mesh.Indices.data(), static_cast<size_t>(indexBufferSize));

    m_indexBuffer = m_device.CreateBuffer(indexBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
    m_device.Context().CopyBuffer(indexStagingBuffer, m_indexBuffer, indexBufferSize);

    m_indexBufferView = Gris::Graphics::Vulkan::BufferView(m_indexBuffer, 0, static_cast<uint32_t>(indexBufferSize));
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateMeshTexture()
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

void HelloTriangleApplication::CreatePipelineStateObject()
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

    if (!m_resourceLayout[UNIQUE_DESCRIPTOR_SET_INDEX])
    {
        auto const resourceLayouts = std::array{
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "ubo",
                0,
                Gris::Graphics::Backend::ShaderResourceType::UniformBuffer,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Vertex,
            },
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "texSampler",
                1,
                Gris::Graphics::Backend::ShaderResourceType::CombinedImageSampler,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Fragment,
            },
        };
        Gris::Graphics::Backend::ShaderResourceBindingsLayout bindingsLayout{ resourceLayouts };
        m_resourceLayout[UNIQUE_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindingsLayout(bindingsLayout);
    }

    ///

    Gris::Graphics::Vulkan::InputLayout layout;
    layout.AddBinding(0, sizeof(Gris::Graphics::Vertex), vk::VertexInputRate::eVertex);
    layout.AddAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Gris::Graphics::Vertex, Position));
    layout.AddAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Gris::Graphics::Vertex, Color));
    layout.AddAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Gris::Graphics::Vertex, TextureCoords));

    m_pso = m_device.CreatePipelineStateObject(m_swapChain.Extent().width, m_swapChain.Extent().height, m_renderPass, layout, m_resourceLayout, m_vertexShader, m_fragmentShader);
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateFramebuffers()
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

void HelloTriangleApplication::CreateShaderResourceBindingsPools()
{
    auto sizes = Gris::Graphics::Backend::ShaderResourceBindingsPoolSizes{};
    sizes.ShaderResourceBindingsCount = m_swapChain.ImageCount();
    sizes.CombinedImageSamplerCount = m_swapChain.ImageCount();
    sizes.UniformBufferCount = m_swapChain.ImageCount();

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

void HelloTriangleApplication::CreateUniformBuffersAndBindings()
{
    m_uniformBuffers.resize(m_swapChain.ImageCount());
    m_uniformBufferViews.resize(m_swapChain.ImageCount());
    m_shaderResourceBindings.resize(m_swapChain.ImageCount());
    for (size_t i = 0; i < m_swapChain.ImageCount(); i++)
    {
        m_uniformBuffers[i] = m_device.CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        m_uniformBufferViews[i] = Gris::Graphics::Vulkan::BufferView(m_uniformBuffers[i], 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));

        m_shaderResourceBindings[i][UNIQUE_DESCRIPTOR_SET_INDEX] = m_device.CreateShaderResourceBindings(m_resourceLayout[UNIQUE_DESCRIPTOR_SET_INDEX]);
        m_shaderResourceBindings[i][UNIQUE_DESCRIPTOR_SET_INDEX].SetCombinedSamplerAndImageView("texSampler", m_meshTextureSampler, m_meshTextureImageView);
        m_shaderResourceBindings[i][UNIQUE_DESCRIPTOR_SET_INDEX].SetUniformBuffer("ubo", m_uniformBufferViews[i]);
        m_shaderResourceBindings[i][UNIQUE_DESCRIPTOR_SET_INDEX].PrepareBindings(m_shaderResourceBindingsPoolCategory, &m_shaderResourceBindingsPools);
    }
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::CreateCommandBuffers()
{
    m_commandBuffers.resize(m_swapChainFramebuffers.size());
    for (uint32_t i = 0; i < m_swapChainFramebuffers.size(); i++)
    {
        m_commandBuffers[i] = m_device.CreateDeferredContext();
        m_commandBuffers[i].Begin();
        m_commandBuffers[i].BeginRenderPass(m_renderPass, m_swapChainFramebuffers[i], m_swapChain.Extent());
        m_commandBuffers[i].BindPipeline(m_pso);
        m_commandBuffers[i].BindVertexBuffer(m_vertexBufferView);
        m_commandBuffers[i].BindIndexBuffer(m_indexBufferView);
        m_commandBuffers[i].BindDescriptorSet(m_pso, 0, m_shaderResourceBindings[i]);
        m_commandBuffers[i].DrawIndexed(static_cast<uint32_t>(m_mesh.Indices.size()));
        m_commandBuffers[i].EndRenderPass();
        m_commandBuffers[i].End();
    }
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto const currentTime = std::chrono::high_resolution_clock::now();
    auto const time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto const swapChainExtent = m_swapChain.Extent();

    UniformBufferObject ubo = {};

    constexpr glm::vec3 EyeLocation = glm::vec3(2.0F, 2.0F, 2.0F);
    constexpr glm::vec3 Origin = glm::vec3(0.0F, 0.0F, 0.0F);
    constexpr glm::vec3 Up = glm::vec3(0.0F, 0.0F, 1.0F);
    constexpr glm::vec3 RotationAxis = glm::vec3(0.0F, 0.0F, 1.0F);
    constexpr float NearPlane = 0.1F;
    constexpr float FarPlane = 10.0F;

    ubo.model = glm::rotate(glm::mat4(1.0F), time * glm::half_pi<float>(), RotationAxis);
    ubo.view = glm::lookAt(EyeLocation, Origin, Up);
    ubo.proj = glm::perspective(glm::quarter_pi<float>(), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), NearPlane, FarPlane);
    ubo.proj[1][1] *= -1;

    m_uniformBuffers[currentImage].SetData(&ubo, sizeof(ubo));
}

// -------------------------------------------------------------------------------------------------

void HelloTriangleApplication::DrawFrame()
{
    auto const nextImageResult = m_swapChain.NextImage();
    if (!nextImageResult)
    {
        CreateVulkanObjects();
        return;
    }

    UpdateUniformBuffer(nextImageResult->SwapChainImageIndex);

    auto const waitSemaphores = std::vector{ std::ref(m_swapChain.ImageAvailableSemaphore(*nextImageResult)) };
    auto const signalSemaphores = std::vector{ std::ref(m_swapChain.RenderingFinishedSemaphore(*nextImageResult)) };
    m_device.Context().Submit(&m_commandBuffers[nextImageResult->SwapChainImageIndex], waitSemaphores, signalSemaphores, m_swapChain.RenderingFinishedFence(*nextImageResult));

    auto const presentResult = m_swapChain.Present(*nextImageResult);
    if (!presentResult || m_framebufferResized)
    {
        CreateVulkanObjects();
        m_framebufferResized = false;
    }
}
