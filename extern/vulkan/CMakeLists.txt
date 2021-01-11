cmake_minimum_required(VERSION 3.17.0)

macro(compile_shader_to_spirv target input_file output_file)
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> ${input_file} -o "${output_file}"
  )
endmacro()

find_package(Vulkan REQUIRED)

# Propagate the Vulkan library
add_library(Gris.Dependencies.Vulkan IMPORTED INTERFACE GLOBAL)
target_compile_definitions(Gris.Dependencies.Vulkan
  INTERFACE GRIS_HAS_VULKAN
)
target_link_libraries(Gris.Dependencies.Vulkan
  INTERFACE Vulkan::Vulkan
)

# Propagate (or create) the glslc executable
add_executable(Gris.Dependencies.glslc IMPORTED GLOBAL)
if(TARGET Vulkan::glslc)
  get_property(GLSLC_LOCATION TARGET Vulkan::glslc PROPERTY IMPORTED_LOCATION)
  set_property(TARGET Gris.Dependencies.glslc PROPERTY IMPORTED_LOCATION "${GLSLC_LOCATION}")
else()
  find_program(GLSLC_LOCATION
    NAMES glslc
    HINTS "$ENV{VULKAN_SDK}/Bin"
  )
  set_property(TARGET Gris.Dependencies.glslc PROPERTY IMPORTED_LOCATION "${GLSLC_LOCATION}")
endif()
