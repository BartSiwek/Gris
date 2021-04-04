cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_executable(Gris.Demos.VulkanTutorial)

target_sources(Gris.Demos.VulkanTutorial PRIVATE
  "src/main.cpp"
  "src/hello_triangle_application.cpp"
  "src/hello_triangle_application.h"
)

target_link_libraries(Gris.Demos.VulkanTutorial PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
  Gris.Graphics
  CONAN_PKG::glfw
  CONAN_PKG::glm
)

###############################################################################

group_sources(Gris.Demos.VulkanTutorial)

###############################################################################

compile_shader_to_spirv(Gris.Demos.VulkanTutorial "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.vert" "$<TARGET_FILE_DIR:Gris.Demos.VulkanTutorial>/vert.spv")
compile_shader_to_spirv(Gris.Demos.VulkanTutorial "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.frag" "$<TARGET_FILE_DIR:Gris.Demos.VulkanTutorial>/frag.spv")

# TODO: Replace this with a resource target(s)... (if that is possible)
add_custom_command(TARGET Gris.Demos.VulkanTutorial
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png"
  $<TARGET_FILE_DIR:Gris.Demos.VulkanTutorial>
)

# TODO: Replace this with a resource target(s)... (if that is possible)
add_custom_command(TARGET Gris.Demos.VulkanTutorial
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.obj"
  $<TARGET_FILE_DIR:Gris.Demos.VulkanTutorial>
)

###############################################################################
