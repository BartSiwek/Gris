cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_library(Gris.Graphics)

target_sources(Gris.Graphics PRIVATE
  "src/gris/graphics/WindowObserver.cpp"
  "src/gris/graphics/glfw/GlfwInstance.cpp"
  "src/gris/graphics/glfw/GlfwWindowMixin.cpp"
  "src/gris/graphics/vulkan/ValidationLayers.h"
  "src/gris/graphics/vulkan/VulkanAllocation.cpp"
  "src/gris/graphics/vulkan/VulkanAllocator.cpp"
  "src/gris/graphics/vulkan/VulkanBuffer.cpp"
  "src/gris/graphics/vulkan/VulkanBufferView.cpp"
  "src/gris/graphics/vulkan/VulkanDeferredContext.cpp"
  "src/gris/graphics/vulkan/VulkanDevice.cpp"
  "src/gris/graphics/vulkan/VulkanDeviceResource.cpp"
  "src/gris/graphics/vulkan/VulkanFence.cpp"
  "src/gris/graphics/vulkan/VulkanFramebuffer.cpp"
  "src/gris/graphics/vulkan/VulkanImmediateContext.cpp"
  "src/gris/graphics/vulkan/VulkanInputLayout.cpp"
  "src/gris/graphics/vulkan/VulkanInstance.cpp"
  "src/gris/graphics/vulkan/VulkanPhysicalDevice.cpp"
  "src/gris/graphics/vulkan/VulkanPhysicalDeviceFactory.cpp"
  "src/gris/graphics/vulkan/VulkanPipelineStateObject.cpp"
  "src/gris/graphics/vulkan/VulkanRenderPass.cpp"
  "src/gris/graphics/vulkan/VulkanSampler.cpp"
  "src/gris/graphics/vulkan/VulkanSemaphore.cpp"
  "src/gris/graphics/vulkan/VulkanShader.cpp"
  "src/gris/graphics/vulkan/VulkanShaderResourceBinding.cpp"
  "src/gris/graphics/vulkan/VulkanSwapChain.cpp"
  "src/gris/graphics/vulkan/VulkanTexture.cpp"
  "src/gris/graphics/vulkan/VulkanTextureView.cpp"
  "src/gris/graphics/vulkan/VulkanUtils.cpp"
  "src/gris/graphics/vulkan/VulkanWindowMixin.cpp"
  "src/gris/graphics/vulkan/vma_implementation.cpp"
  "src/gris/graphics/vulkan/glfw/GlfwExtensionGetters.cpp"
  "src/gris/graphics/vulkan/glfw/GlfwVulkanWindow.cpp"
  "include/gris/graphics/EngineException.h"
  "include/gris/graphics/WindowObserver.h"
  "include/gris/graphics/glfw/GlfwInstance.h"
  "include/gris/graphics/glfw/GlfwWindowMixin.h"
  "include/gris/graphics/vulkan/VulkanAllocation.h"
  "include/gris/graphics/vulkan/VulkanAllocator.h"
  "include/gris/graphics/vulkan/VulkanBuffer.h"
  "include/gris/graphics/vulkan/VulkanBufferView.h"
  "include/gris/graphics/vulkan/VulkanDeferredContext.h"
  "include/gris/graphics/vulkan/VulkanDevice.h"
  "include/gris/graphics/vulkan/VulkanDeviceResource.h"
  "include/gris/graphics/vulkan/VulkanEngineException.h"
  "include/gris/graphics/vulkan/VulkanFence.h"
  "include/gris/graphics/vulkan/VulkanFramebuffer.h"
  "include/gris/graphics/vulkan/VulkanInstance.h"
  "include/gris/graphics/vulkan/VulkanImmediateContext.h"
  "include/gris/graphics/vulkan/VulkanInputLayout.h"
  "include/gris/graphics/vulkan/VulkanPhysicalDevice.h"
  "include/gris/graphics/vulkan/VulkanPhysicalDeviceFactory.h"
  "include/gris/graphics/vulkan/VulkanPipelineStateObject.h"
  "include/gris/graphics/vulkan/VulkanPipelineStateObjectResource.h"
  "include/gris/graphics/vulkan/VulkanRenderPass.h"
  "include/gris/graphics/vulkan/VulkanSampler.h"
  "include/gris/graphics/vulkan/VulkanSemaphore.h"
  "include/gris/graphics/vulkan/VulkanShader.h"
  "include/gris/graphics/vulkan/VulkanShaderResourceBinding.h"
  "include/gris/graphics/vulkan/VulkanSwapChain.h"
  "include/gris/graphics/vulkan/VulkanTexture.h"
  "include/gris/graphics/vulkan/VulkanTextureView.h"
  "include/gris/graphics/vulkan/VulkanUtils.h"
  "include/gris/graphics/vulkan/VulkanWindowMixin.h"
  "include/gris/graphics/vulkan/glfw/GlfwExtensionGetters.h"
  "include/gris/graphics/vulkan/glfw/GlfwVulkanWindow.h"
)

target_include_directories(Gris.Graphics PUBLIC
  "include"
)

target_link_libraries(Gris.Graphics PUBLIC
  Gris.Dependencies.Vulkan
  CONAN_PKG::vulkan-memory-allocator
  CONAN_PKG::glfw
)

target_link_libraries(Gris.Graphics PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
)

########################################################################

group_sources(Gris.Graphics)

########################################################################

add_subdirectory(tests)
