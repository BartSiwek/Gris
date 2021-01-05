cmake_minimum_required(VERSION 3.17.0)

########################################################################

include(test_compile_success)
include(test_compile_fail)

if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

########################################################################

add_executable(Gris.Core.Assert.Tests)

target_sources(Gris.Core.Assert.Tests PRIVATE
  "main.cpp"
  "test_always_fast_slow.cpp"
  "test_no_always_fast_slow.cpp"
  "test_always_no_fast_slow.cpp"
  "test_always_fast_no_slow.cpp"
  "test_always_no_fast_no_slow.cpp"
  "test_no_always_fast_no_slow.cpp"
  "test_no_always_no_fast_slow.cpp"
  "test_no_always_no_fast_no_slow.cpp"
)

target_link_libraries(Gris.Core.Assert.Tests PRIVATE
  Gris.ProjectOptions
  Gris.ProjectWarnings
  Gris.Core
  CONAN_PKG::catch2
)

catch_discover_tests(
  Gris.Core.Assert.Tests
  TEST_PREFIX
  "[Gris.Core.Assert.Tests]"
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=Gris.Core.Assert.Tests.xml
)

###############################################################################

if(GRIS_ASSERT_COMPILATION_TESTS)
  # Test target level assertion settings
  test_compile_success(test_assert_defaults_tu_defaults "compilation/assert_tu_defaults.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_defaults "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_defaults "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_defaults "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_defaults "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  # TU level assertion settings
  test_compile_success(test_assert_defaults_tu_none "compilation/assert_tu_none.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_none_tu_none "compilation/assert_tu_none.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_none "compilation/assert_tu_none.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_debug_tu_none "compilation/assert_tu_none.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_safe_tu_none "compilation/assert_tu_none.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_none.h\"")

  test_compile_success(test_assert_defaults_tu_always "compilation/assert_tu_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_always "compilation/assert_tu_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_release_tu_always "compilation/assert_tu_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_always "compilation/assert_tu_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_always "compilation/assert_tu_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_disable_always "compilation/assert_tu_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_none_tu_disable_always "compilation/assert_tu_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_always "compilation/assert_tu_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_debug_tu_disable_always "compilation/assert_tu_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_always "compilation/assert_tu_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast_slow.h\"")

  test_compile_success(test_assert_defaults_tu_fast "compilation/assert_tu_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_none_tu_fast "compilation/assert_tu_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_release_tu_fast "compilation/assert_tu_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_debug_tu_fast "compilation/assert_tu_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_fast "compilation/assert_tu_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_fast_always "compilation/assert_tu_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_none_tu_fast_always "compilation/assert_tu_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_release_tu_fast_always "compilation/assert_tu_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_debug_tu_fast_always "compilation/assert_tu_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_fast_always "compilation/assert_tu_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_fast_disable_always "compilation/assert_tu_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_none_tu_fast_disable_always "compilation/assert_tu_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_release_tu_fast_disable_always "compilation/assert_tu_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_debug_tu_fast_disable_always "compilation/assert_tu_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_safe_tu_fast_disable_always "compilation/assert_tu_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast_slow.h\"")

  test_compile_success(test_assert_defaults_tu_disable_fast "compilation/assert_tu_disable_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_fast "compilation/assert_tu_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_fast "compilation/assert_tu_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_fast "compilation/assert_tu_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_safe_tu_disable_fast "compilation/assert_tu_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_slow.h\"")

  test_compile_success(test_assert_defaults_tu_disable_fast_always "compilation/assert_tu_disable_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_fast_always "compilation/assert_tu_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_release_tu_disable_fast_always "compilation/assert_tu_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_fast_always "compilation/assert_tu_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_safe_tu_disable_fast_always "compilation/assert_tu_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_slow.h\"")

  test_compile_success(test_assert_defaults_tu_disable_fast_disable_always "compilation/assert_tu_disable_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_none_tu_disable_fast_disable_always "compilation/assert_tu_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_fast_disable_always "compilation/assert_tu_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_debug_tu_disable_fast_disable_always "compilation/assert_tu_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_safe_tu_disable_fast_disable_always "compilation/assert_tu_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_slow.h\"")

  test_compile_success(test_assert_defaults_tu_slow "compilation/assert_tu_slow.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_none_tu_slow "compilation/assert_tu_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_release_tu_slow "compilation/assert_tu_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow "compilation/assert_tu_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_safe_tu_slow "compilation/assert_tu_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_slow_always "compilation/assert_tu_slow_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_always "compilation/assert_tu_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_always "compilation/assert_tu_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_always "compilation/assert_tu_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_safe_tu_slow_always "compilation/assert_tu_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_slow_disable_always "compilation/assert_tu_slow_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_disable_always "compilation/assert_tu_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_disable_always "compilation/assert_tu_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_disable_always "compilation/assert_tu_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_safe_tu_slow_disable_always "compilation/assert_tu_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast_slow.h\"")

  test_compile_success(test_assert_defaults_tu_slow_fast "compilation/assert_tu_slow_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_none_tu_slow_fast "compilation/assert_tu_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_fast "compilation/assert_tu_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_debug_tu_slow_fast "compilation/assert_tu_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_safe_tu_slow_fast "compilation/assert_tu_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_slow_fast_always "compilation/assert_tu_slow_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_none_tu_slow_fast_always "compilation/assert_tu_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_release_tu_slow_fast_always "compilation/assert_tu_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_debug_tu_slow_fast_always "compilation/assert_tu_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_all.h\"")
  test_compile_success(test_assert_safe_tu_slow_fast_always "compilation/assert_tu_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_all.h\"")

  test_compile_success(test_assert_defaults_tu_slow_fast_disable_always "compilation/assert_tu_slow_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_fast_disable_always "compilation/assert_tu_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_fast_disable_always "compilation/assert_tu_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_fast_disable_always "compilation/assert_tu_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast_slow.h\"")
  test_compile_success(test_assert_safe_tu_slow_fast_disable_always "compilation/assert_tu_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast_slow.h\"")

  test_compile_success(test_assert_defaults_tu_slow_disable_fast "compilation/assert_tu_slow_disable_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_disable_fast "compilation/assert_tu_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_disable_fast "compilation/assert_tu_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_disable_fast "compilation/assert_tu_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_safe_tu_slow_disable_fast "compilation/assert_tu_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_slow.h\"")

  test_compile_success(test_assert_defaults_tu_slow_disable_fast_always "compilation/assert_tu_slow_disable_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_disable_fast_always "compilation/assert_tu_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_disable_fast_always "compilation/assert_tu_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_disable_fast_always "compilation/assert_tu_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_slow.h\"")
  test_compile_success(test_assert_safe_tu_slow_disable_fast_always "compilation/assert_tu_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_slow.h\"")

  test_compile_success(test_assert_defaults_tu_slow_disable_fast_disable_always "compilation/assert_tu_slow_disable_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_none_tu_slow_disable_fast_disable_always "compilation/assert_tu_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_release_tu_slow_disable_fast_disable_always "compilation/assert_tu_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_debug_tu_slow_disable_fast_disable_always "compilation/assert_tu_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_slow.h\"")
  test_compile_success(test_assert_safe_tu_slow_disable_fast_disable_always "compilation/assert_tu_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_slow.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow "compilation/assert_tu_disable_slow.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_slow "compilation/assert_tu_disable_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_slow "compilation/assert_tu_disable_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow "compilation/assert_tu_disable_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow "compilation/assert_tu_disable_slow.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_always "compilation/assert_tu_disable_slow_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_always "compilation/assert_tu_disable_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_always "compilation/assert_tu_disable_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_always "compilation/assert_tu_disable_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_always "compilation/assert_tu_disable_slow_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_disable_always "compilation/assert_tu_disable_slow_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_disable_always "compilation/assert_tu_disable_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_disable_always "compilation/assert_tu_disable_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_disable_always "compilation/assert_tu_disable_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_disable_always "compilation/assert_tu_disable_slow_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_fast "compilation/assert_tu_disable_slow_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_fast "compilation/assert_tu_disable_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_fast "compilation/assert_tu_disable_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_fast "compilation/assert_tu_disable_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_fast "compilation/assert_tu_disable_slow_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_fast_always "compilation/assert_tu_disable_slow_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_fast_always "compilation/assert_tu_disable_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_fast_always "compilation/assert_tu_disable_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_fast_always "compilation/assert_tu_disable_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_fast_always "compilation/assert_tu_disable_slow_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_fast_disable_always "compilation/assert_tu_disable_slow_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_fast_disable_always "compilation/assert_tu_disable_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_fast_disable_always "compilation/assert_tu_disable_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_fast_disable_always "compilation/assert_tu_disable_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_fast.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_fast_disable_always "compilation/assert_tu_disable_slow_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_fast.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_disable_fast "compilation/assert_tu_disable_slow_disable_fast.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_disable_fast "compilation/assert_tu_disable_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_disable_fast "compilation/assert_tu_disable_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_disable_fast "compilation/assert_tu_disable_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_disable_fast "compilation/assert_tu_disable_slow_disable_fast.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_disable_fast_always "compilation/assert_tu_disable_slow_disable_fast_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_disable_fast_always "compilation/assert_tu_disable_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_disable_fast_always "compilation/assert_tu_disable_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_disable_fast_always "compilation/assert_tu_disable_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_always.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_disable_fast_always "compilation/assert_tu_disable_slow_disable_fast_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_always.h\"")

  test_compile_success(test_assert_defaults_tu_disable_slow_disable_fast_disable_always "compilation/assert_tu_disable_slow_disable_fast_disable_always.cpp" "Gris.Core" "EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_none_tu_disable_slow_disable_fast_disable_always "compilation/assert_tu_disable_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_release_tu_disable_slow_disable_fast_disable_always "compilation/assert_tu_disable_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_debug_tu_disable_slow_disable_fast_disable_always "compilation/assert_tu_disable_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_none.h\"")
  test_compile_success(test_assert_safe_tu_disable_slow_disable_fast_disable_always "compilation/assert_tu_disable_slow_disable_fast_disable_always.cpp" "Gris.Core" "GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_none.h\"")

  # Target level error cases
  test_compile_fail(test_assert_none_and_release_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;GRIS_TARGET_MODE_RELEASE;EXPECT_HEADER=\"expect_nothing.h\"")
  test_compile_fail(test_assert_none_and_debug_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_nothing.h\"")
  test_compile_fail(test_assert_none_and_safe_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_NONE;GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_nothing.h\"")
  test_compile_fail(test_assert_release_and_debug_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;GRIS_TARGET_MODE_DEBUG;EXPECT_HEADER=\"expect_nothing.h\"")
  test_compile_fail(test_assert_release_and_safe_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_RELEASE;GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_nothing.h\"")
  test_compile_fail(test_assert_debug_and_safe_fails "compilation/assert_tu_defaults.cpp" "Gris.Core" "GRIS_TARGET_MODE_DEBUG;GRIS_TARGET_MODE_SAFE;EXPECT_HEADER=\"expect_nothing.h\"")

  # TU-level error cases
  test_compile_fail(test_assert_tu_none_and_always_fails "compilation/assert_tu_none_and_always.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_none_and_disable_always_fails "compilation/assert_tu_none_and_disable_always.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_none_and_fast_fails "compilation/assert_tu_none_and_fast.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_none_and_disable_fast_fails "compilation/assert_tu_none_and_disable_fast.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_none_and_slow_fails "compilation/assert_tu_none_and_slow.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_none_and_disable_slow_fails "compilation/assert_tu_none_and_disable_slow.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_always_and_disable_always_fails "compilation/assert_tu_none_and_disable_always.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_fast_and_disable_fast_fails "compilation/assert_tu_fast_and_disable_fast.cpp" "Gris.Core" "")
  test_compile_fail(test_assert_tu_slow_and_disable_slow_fails "compilation/assert_tu_slow_and_disable_slow.cpp" "Gris.Core" "")
endif()
