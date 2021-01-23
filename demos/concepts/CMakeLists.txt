cmake_minimum_required(VERSION 3.17.0)

########################################################################

add_executable(Gris.Demos.Concepts)

target_sources(Gris.Demos.Concepts PRIVATE
  "src/main.cpp"
)

target_link_libraries(Gris.Demos.Concepts PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
)

###############################################################################

group_sources(Gris.Demos.Concepts)

########################################################################
