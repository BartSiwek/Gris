macro(run_conan requires options imports)
  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  conan_add_remote(
    NAME
    bincrafters
    URL
    https://api.bintray.com/conan/bincrafters/public-conan
  )

  conan_add_remote(
    NAME
    catchorg
    URL
    https://api.bintray.com/conan/catchorg/Catch2
  )

  conan_cmake_run(
    REQUIRES
      ${requires}
    OPTIONS
      ${options}
    IMPORTS
      ${imports}
    BASIC_SETUP
    CMAKE_TARGETS # individual targets to link to
    BUILD
      missing
    ENV CC=${CMAKE_C_COMPILER}
    ENV CXX=${CMAKE_CXX_COMPILER}
  )
endmacro()