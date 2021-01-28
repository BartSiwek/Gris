cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_library(Gris.Graphics)

target_sources(Gris.Graphics PRIVATE
  "src/gris/graphics/window_observer.cpp"
  "src/gris/graphics/vulkan/validation_layers.h"
  "src/gris/graphics/vulkan/allocation.cpp"
  "src/gris/graphics/vulkan/allocator.cpp"
  "src/gris/graphics/vulkan/buffer.cpp"
  "src/gris/graphics/vulkan/buffer_view.cpp"
  "src/gris/graphics/vulkan/deferred_context.cpp"
  "src/gris/graphics/vulkan/device.cpp"
  "src/gris/graphics/vulkan/device_resource.cpp"
  "src/gris/graphics/vulkan/fence.cpp"
  "src/gris/graphics/vulkan/framebuffer.cpp"
  "src/gris/graphics/vulkan/immediate_context.cpp"
  "src/gris/graphics/vulkan/input_layout.cpp"
  "src/gris/graphics/vulkan/instance.cpp"
  "src/gris/graphics/vulkan/physical_device.cpp"
  "src/gris/graphics/vulkan/physical_device_factory.cpp"
  "src/gris/graphics/vulkan/pipeline_resource_group_layout.cpp"
  "src/gris/graphics/vulkan/pipeline_state_object.cpp"
  "src/gris/graphics/vulkan/render_pass.cpp"
  "src/gris/graphics/vulkan/sampler.cpp"
  "src/gris/graphics/vulkan/semaphore.cpp"
  "src/gris/graphics/vulkan/shader.cpp"
  "src/gris/graphics/vulkan/shader_resource_binding.cpp"
  "src/gris/graphics/vulkan/swap_chain.cpp"
  "src/gris/graphics/vulkan/texture.cpp"
  "src/gris/graphics/vulkan/texture_view.cpp"
  "src/gris/graphics/vulkan/utils.cpp"
  "src/gris/graphics/vulkan/vma_implementation.cpp"
  "src/gris/graphics/vulkan/window_mixin.cpp"
  "include/gris/graphics/window_observer.h"
  "include/gris/graphics/vulkan/allocation.h"
  "include/gris/graphics/vulkan/allocator.h"
  "include/gris/graphics/vulkan/buffer.h"
  "include/gris/graphics/vulkan/buffer_view.h"
  "include/gris/graphics/vulkan/deferred_context.h"
  "include/gris/graphics/vulkan/device.h"
  "include/gris/graphics/vulkan/device_resource.h"
  "include/gris/graphics/vulkan/vulkan_engine_exception.h"
  "include/gris/graphics/vulkan/fence.h"
  "include/gris/graphics/vulkan/framebuffer.h"
  "include/gris/graphics/vulkan/instance.h"
  "include/gris/graphics/vulkan/immediate_context.h"
  "include/gris/graphics/vulkan/input_layout.h"
  "include/gris/graphics/vulkan/physical_device.h"
  "include/gris/graphics/vulkan/physical_device_factory.h"
  "include/gris/graphics/vulkan/pipeline_resource_group_layout.h"
  "include/gris/graphics/vulkan/pipeline_state_object.h"
  "include/gris/graphics/vulkan/render_pass.h"
  "include/gris/graphics/vulkan/sampler.h"
  "include/gris/graphics/vulkan/semaphore.h"
  "include/gris/graphics/vulkan/shader.h"
  "include/gris/graphics/vulkan/shader_resource_binding.h"
  "include/gris/graphics/vulkan/swap_chain.h"
  "include/gris/graphics/vulkan/texture.h"
  "include/gris/graphics/vulkan/texture_view.h"
  "include/gris/graphics/vulkan/utils.h"
  "include/gris/graphics/vulkan/vma_headers.h"
  "include/gris/graphics/vulkan/vulkan_headers.h"
  "include/gris/graphics/vulkan/window_mixin.h"
)

target_include_directories(Gris.Graphics PUBLIC
  "include"
)

target_link_libraries(Gris.Graphics PUBLIC
  CONAN_PKG::vulkan-memory-allocator
  CONAN_PKG::vulkan-headers
  CONAN_PKG::glfw
)

target_link_libraries(Gris.Graphics PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
)

if(ENABLE_GLFW)
  target_sources(Gris.Graphics PRIVATE
    "src/gris/graphics/glfw/instance.cpp"
    "src/gris/graphics/glfw/window_mixin.cpp"
    "src/gris/graphics/vulkan/glfw/extension_getters.h"
    "src/gris/graphics/vulkan/glfw/extension_getters.cpp"
    "src/gris/graphics/vulkan/glfw/window.cpp"
    "include/gris/graphics/glfw/instance.h"
    "include/gris/graphics/glfw/window_mixin.h"
    "include/gris/graphics/vulkan/glfw/window.h"
  )

  target_compile_definitions(Gris.Graphics PRIVATE
    GRIS_CHRAPHICS_HAS_GLFW=1
  )
endif()

########################################################################

group_sources(Gris.Graphics)

########################################################################

add_subdirectory(tests)
