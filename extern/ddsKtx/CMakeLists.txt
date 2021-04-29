cmake_minimum_required(VERSION 3.17.0)

include(FetchContent)

FetchContent_Declare(
  dds-ktx
  GIT_REPOSITORY https://github.com/septag/dds-ktx.git
  GIT_TAG master
)

FetchContent_GetProperties(dds-ktx)
if(NOT dds-ktx_POPULATED)
  FetchContent_Populate(dds-ktx)
  add_library(Gris.Dependencies.Dds-Ktx IMPORTED INTERFACE GLOBAL)
  target_sources(Gris.Dependencies.Dds-Ktx INTERFACE "${dds-ktx_SOURCE_DIR}/dds-ktx.h")
  target_include_directories(Gris.Dependencies.Dds-Ktx INTERFACE "${dds-ktx_SOURCE_DIR}")
endif()
