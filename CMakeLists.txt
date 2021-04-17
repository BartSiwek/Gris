cmake_minimum_required(VERSION 3.17.0)
project(Gris VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

########################################################################

include(CTest)
include(group_sources_by_folder)
include(standard_project_settings)
include(compiler_warnings)
include(conan)

########################################################################

option(GRIS_ASSERT_COMPILATION_TESTS "Enable or disable Gris compilation assert tests" OFF)

option(ENABLE_GLFW "Enable or disable use of GLFW for window management" ON)

set(GRIS_LOG_LEVEL INFO CACHE STRING "Set minimum enabled logging level")
set_property(CACHE GRIS_LOG_LEVEL PROPERTY STRINGS TRACE DEBUG INFO WARNING ERROR CRITICAL OFF)

########################################################################

enable_testing()

########################################################################

setup_standard_project_settings()

add_library(Gris.ProjectOptions INTERFACE)
target_compile_features(Gris.ProjectOptions INTERFACE cxx_std_17)

add_library(Gris.ProjectWarnings INTERFACE)
set_project_warnings(Gris.ProjectWarnings)

########################################################################

set(CONAN_REQUIRES
  fmt/7.1.2
  spdlog/1.8.2
  catch2/2.13.2
  span-lite/0.9.0
  glfw/3.3.2
  glm/0.9.9.8
  stb/20200203
  tinyobjloader/1.0.6
  assimp/5.0.1
  vulkan-headers/1.2.154.0
  vulkan-memory-allocator/2.3.0
)

set(CONAN_OPTIONS "")

set(CONAN_IMPORTS
  "bin, *.dll -> ./bin"
)

if(ENABLE_GLFW)
  list(APPEND CONAN_REQUIRES
    glfw/3.3.2
  )
endif()

run_conan("${CONAN_REQUIRES}" "${CONAN_OPTIONS}" "${CONAN_IMPORTS}")

####################################################################

if(MSVC)
  # Quaternion code in glm causes this warning to fire
  target_compile_options(CONAN_PKG::glm INTERFACE "/wd4201")
endif()

target_compile_definitions(CONAN_PKG::glm INTERFACE GLM_ENABLE_EXPERIMENTAL)

########################################################################

add_subdirectory(extern/vulkan)

########################################################################

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(demos)
