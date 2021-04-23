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

compile_shader_to_spirv(Gris.Demos.ForwardRendering "${PROJECT_SOURCE_DIR}/resources" "shaders/demos/forward_rendering/shader.vert" "$<CONFIG>/vertex.spv")
compile_shader_to_spirv(Gris.Demos.ForwardRendering "${PROJECT_SOURCE_DIR}/resources" "shaders/demos/forward_rendering/shader.frag" "$<CONFIG>/fragment.spv")

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
  "${PROJECT_SOURCE_DIR}/resources/models/sponza/sponza.dae"
  $<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>
)

create_resource_target(Gris.Demos.ForwardRendering)

###############################################################################
