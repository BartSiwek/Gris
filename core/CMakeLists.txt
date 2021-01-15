cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_library(Gris.Core)

target_sources(Gris.Core PRIVATE
  "src/gris/assert.cpp"
  "src/gris/directory_registry.cpp"
  "include/gris/assert.h"
  "include/gris/directory_registry.h"
  "include/gris/macros.h"
  "include/gris/engine_exception.h"
)

target_include_directories(Gris.Core PUBLIC
  "include"
)

target_link_libraries(Gris.Core PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
)

target_link_libraries(Gris.Core PUBLIC
  CONAN_PKG::fmt
)

########################################################################

group_sources(Gris.Core)

########################################################################

add_subdirectory(tests)
