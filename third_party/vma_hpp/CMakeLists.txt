cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_library(Gris.Dependencies.VmaHpp)

target_sources(Gris.Dependencies.VmaHpp PRIVATE
  "src/vk_mem_implementation.cpp"
  "include/vk_mem_alloc.h"
  "include/vk_mem_alloc.hpp"
)

target_include_directories(Gris.Dependencies.VmaHpp SYSTEM PUBLIC
  "include"
)

target_link_libraries(Gris.Dependencies.VmaHpp PUBLIC
  Gris.Demos.VulkanTutorial
)
