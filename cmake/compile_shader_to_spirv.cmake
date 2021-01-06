macro(compile_shader_to_spirv target input_file output_file)
  find_package(Vulkan QUIET)
  if(TARGET Vulkan::glslc)
    add_custom_command(TARGET ${target}
      POST_BUILD
      COMMAND $<TARGET_FILE:Vulkan::glslc> ${input_file} -o "${output_file}"
    )
  endif()
endmacro()
