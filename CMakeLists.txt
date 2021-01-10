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
  catch2/2.13.2
  glfw/3.3.2
  glm/0.9.9.8
  stb/20200203
  tinyobjloader/1.0.6
  vulkan-memory-allocator/2.3.0
)
set(CONAN_OPTIONS "")
set(CONAN_IMPORTS
  "bin, *.dll -> ./bin"
)
run_conan("${CONAN_REQUIRES}" "${CONAN_OPTIONS}" "${CONAN_IMPORTS}")

########################################################################

add_subdirectory(extern/vulkan)

########################################################################

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(demos)
