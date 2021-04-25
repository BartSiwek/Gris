cmake_minimum_required(VERSION 3.17.0)

include(FetchContent)

FetchContent_Declare(
  gli
  GIT_REPOSITORY https://github.com/BartSiwek/gli.git
  GIT_TAG        master
)

FetchContent_GetProperties(gli)
if(NOT gli_POPULATED)
  FetchContent_Populate(gli)
  set(GLI_TEST_ENABLE  OFF)
  add_subdirectory(${gli_SOURCE_DIR} ${gli_BINARY_DIR})
endif()
