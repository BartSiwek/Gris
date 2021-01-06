function(group_sources target)
  get_target_property(source_dir ${target} SOURCE_DIR)
  get_target_property(sources ${target} SOURCES)

  source_group(TREE ${source_dir} FILES ${sources})
endfunction()
