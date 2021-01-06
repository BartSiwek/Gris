cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_executable(Gris.Demos.Basic)

target_sources(Gris.Demos.Basic PRIVATE
  "src/main.cpp"
)

target_link_libraries(Gris.Demos.Basic PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
)

###############################################################################

group_sources(Gris.Demos.Basic)

########################################################################
