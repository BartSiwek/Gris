cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_subdirectory(basic)

if(ENABLE_GLFW)
  add_subdirectory(vulkan_tutorial)
endif()
