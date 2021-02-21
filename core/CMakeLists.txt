cmake_minimum_required(VERSION 3.17.0)

########################################################################

set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_INFO")
if(GRIS_LOG_LEVEL STREQUAL "TRACE")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_TRACE")
elseif(GRIS_LOG_LEVEL STREQUAL "DEBUG")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_DEBUG")
elseif(GRIS_LOG_LEVEL STREQUAL "INFO")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_INFO")
elseif(GRIS_LOG_LEVEL STREQUAL "WARNING")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_WARNING")
elseif(GRIS_LOG_LEVEL STREQUAL "ERROR")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVELERROR_")
elseif(GRIS_LOG_LEVEL STREQUAL "CRITICAL")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_CRITICAL")
elseif(GRIS_LOG_LEVEL STREQUAL "OFF")
  set(GRIS_ACTIVE_LOG_LEVEL "GRIS_LEVEL_OFF")
endif()

########################################################################

add_library(Gris.Core)

target_sources(Gris.Core PRIVATE
  "src/gris/assert.cpp"
  "src/gris/directory_registry.cpp"
  "include/gris/assert.h"
  "include/gris/casts.h"
  "include/gris/directory_registry.h"
  "include/gris/engine_exception.h"
  "include/gris/enum.h"
  "include/gris/log.h"
  "include/gris/macros.h"
  "include/gris/object_hierarchy.h"
  "include/gris/span.h"
  "include/gris/strong_type.h"
  "include/gris/utils.h"
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
  CONAN_PKG::spdlog
  CONAN_PKG::span-lite
)

target_compile_definitions(Gris.Core PUBLIC
  GRIS_ACTIVE_LOG_LEVEL=${GRIS_ACTIVE_LOG_LEVEL}
)

########################################################################

group_sources(Gris.Core)

########################################################################

add_subdirectory(tests)
