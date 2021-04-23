cmake_minimum_required(VERSION 3.17.0)

function(get_resource_target_name target result)
  set(${result} "${target}.Resources" PARENT_SCOPE)
endfunction()

function(compile_shader_to_spirv target input_base_path input_suffix_path output_path)
  set(input_path "${input_base_path}/${input_suffix_path}")

  add_custom_command(
    OUTPUT ${output_path}
    COMMAND $<TARGET_FILE:Gris.Dependencies.glslc> -o ${output_path} ${input_path}
    DEPENDS ${input_path}
    IMPLICIT_DEPENDS CXX ${input_path}
    VERBATIM
  )

  set_property(TARGET ${target}
    APPEND PROPERTY GRIS_RESOURCE_OUTPUT_FILES ${output_path}
  )

  target_sources(${target} PRIVATE ${input_path})
  source_group(TREE ${input_base_path} PREFIX resources FILES ${input_path})
endfunction()

function(create_resource_target target)
  get_resource_target_name(${target} resource_target)
  get_target_property(resource_output_files ${target} GRIS_RESOURCE_OUTPUT_FILES)
  add_custom_target(${resource_target} DEPENDS ${resource_output_files})
  add_dependencies(${target} ${resource_target})
endfunction()

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
