cmake_minimum_required(VERSION 3.17.0)
cmake_policy(SET CMP0112 NEW)

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

set(assets_dir "$<TARGET_FILE_DIR:Gris.Demos.VulkanTutorial>/vulkan_tutorial/assets")

# TODO: Revise this code when https://gitlab.kitware.com/cmake/cmake/-/issues/12877 is fixed
add_custom_target(Gris.Demos.VulkanTutorial.Resources
  COMMAND ${CMAKE_COMMAND} -E make_directory  "${assets_dir}"
  COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> -o "${assets_dir}/vertex.spv" "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.vert"
  COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> -o "${assets_dir}/fragment.spv" "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.frag"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png" "${assets_dir}/viking_room.png"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.obj" "${assets_dir}/viking_room.obj"
)

target_sources(Gris.Demos.VulkanTutorial.Resources PRIVATE
  "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.vert"
  "${PROJECT_SOURCE_DIR}/resources/shaders/demos/vulkan_tutorial/shader.frag"
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.png"
  "${PROJECT_SOURCE_DIR}/resources/models/viking_room/viking_room.obj"
)

group_sources_with_base(Gris.Demos.VulkanTutorial.Resources "${PROJECT_SOURCE_DIR}/resources")

add_dependencies(Gris.Demos.VulkanTutorial Gris.Demos.VulkanTutorial.Resources)

###############################################################################

if(DEFINED ENV{VULKAN_SDK})
  set_target_properties(Gris.Demos.VulkanTutorial PROPERTIES
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$ENV{VULKAN_SDK}/lib"
  )
endif()

###############################################################################
