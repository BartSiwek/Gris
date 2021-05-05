cmake_minimum_required(VERSION 3.17.0)
cmake_policy(SET CMP0112 NEW)

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

set(resource_target "Gris.Demos.ForwardRendering.Resources")
set(assets_dir "$<TARGET_FILE_DIR:Gris.Demos.ForwardRendering>/forward_rendering/assets")

# TODO: Revise this code when https://gitlab.kitware.com/cmake/cmake/-/issues/12877 is fixed
add_custom_target(${resource_target}
  COMMAND ${CMAKE_COMMAND} -E make_directory  "${assets_dir}"
  COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> -o "${assets_dir}/vertex.spv" "${PROJECT_SOURCE_DIR}/resources/shaders/demos/forward_rendering/shader.vert"
  COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> -o "${assets_dir}/fragment.spv" "${PROJECT_SOURCE_DIR}/resources/shaders/demos/forward_rendering/shader.frag"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png" "${assets_dir}/viking_room.png"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/resources/models/sponza/sponza.dae" "${assets_dir}/sponza.dae"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/resources/models/sponza/sponza/lion.dds" "${assets_dir}/sponza/lion.dds"
)

target_sources(${resource_target} PRIVATE
  "${PROJECT_SOURCE_DIR}/resources/shaders/demos/forward_rendering/shader.vert"
  "${PROJECT_SOURCE_DIR}/resources/shaders/demos/forward_rendering/shader.frag"
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png"
  "${PROJECT_SOURCE_DIR}/resources/models/sponza/sponza.dae"
)

group_sources_with_base(${resource_target} "${PROJECT_SOURCE_DIR}/resources")

add_dependencies(Gris.Demos.ForwardRendering ${resource_target})

###############################################################################

if(DEFINED ENV{VULKAN_SDK})
  set_target_properties(Gris.Demos.ForwardRendering PROPERTIES
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$ENV{VULKAN_SDK}/lib"
  )
endif()

###############################################################################