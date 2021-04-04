cmake_minimum_required(VERSION 3.17.0)

########################################################################

if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

########################################################################

add_executable(Gris.Graphics.Tests)

target_sources(Gris.Graphics.Tests PRIVATE
  "src/main.cpp"
  "src/test_trackball_camera.cpp"
)

target_link_libraries(Gris.Graphics.Tests PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Graphics
  CONAN_PKG::catch2
)

catch_discover_tests(
  Gris.Graphics.Tests
  TEST_PREFIX
  "[Gris.Graphics.Tests]"
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=Gris.Graphics.Tests.xml
)

########################################################################

group_sources(Gris.Graphics.Tests)

########################################################################
