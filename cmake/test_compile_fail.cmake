function(test_compile_fail testname sourcefiles dependencies definitions)
  add_executable(${testname})

  foreach(sourcefile ${sourcefiles})
    target_sources(${testname} PRIVATE ${sourcefile})
  endforeach()

  target_link_libraries(${testname} "${dependencies}")

  set_target_properties(${testname} PROPERTIES EXCLUDE_FROM_ALL true
                                               EXCLUDE_FROM_DEFAULT_BUILD true)

  add_test(NAME ${testname} COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${testname} --config $<CONFIGURATION>)

  set_tests_properties(${testname} PROPERTIES WILL_FAIL true)

  foreach(defintion ${definitions})
    target_compile_definitions(${testname} PRIVATE ${defintion})
  endforeach()
endfunction()
