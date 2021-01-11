cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_library(Gris.Core)

target_sources(Gris.Core PRIVATE
  "src/gris/assert.cpp"
  "src/gris/directory_registry.cpp"
  "include/gris/assert.h"
  "include/gris/directory_registry.h"
  "include/gris/macros.h"
)

target_include_directories(Gris.Core PUBLIC
  "include"
)

target_link_libraries(Gris.Core PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
)

########################################################################

group_sources(Gris.Core)

########################################################################

add_subdirectory(tests)
