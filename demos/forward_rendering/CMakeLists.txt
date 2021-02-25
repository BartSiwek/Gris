cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_executable(Gris.Demos.ForwardRendering)

target_sources(Gris.Demos.ForwardRendering PRIVATE
  "src/main.cpp"
  "src/forward_rendering_application.cpp"
  "src/forward_rendering_application.h"
)

target_link_libraries(Gris.Demos.ForwardRendering PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
  Gris.Graphics
  CONAN_PKG::glm
)

###############################################################################

group_sources(Gris.Demos.ForwardRendering)

###############################################################################

compile_shader_to_spirv(Gris.Demos.ForwardRendering "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.vert" "$<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>/vert.spv")
compile_shader_to_spirv(Gris.Demos.ForwardRendering "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.frag" "$<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>/frag.spv")

# TODO: Replace this with a resource target(s)... (if that is possible)
add_custom_command(TARGET Gris.Demos.ForwardRendering
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png"
  $<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>
)

# TODO: Replace this with a resource target(s)... (if that is possible)
add_custom_command(TARGET Gris.Demos.ForwardRendering
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.obj"
  $<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>
)

###############################################################################
