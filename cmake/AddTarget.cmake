include(CTest)

function(add_range_target name rangeLib)
  if (NOT TARGET ${rangeLib})
    message(WARNING "No target ${rangeLib}")
    return()
  endif()
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} ${rangeLib} concepts)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  string(REGEX REPLACE ".*::" "" macroName ${rangeLib})
  string(REPLACE "-" "_" macroName ${macroName})
  string(TOUPPER USE_${macroName} macroName)
  target_compile_definitions(${name} PRIVATE ${macroName})
endfunction()

function(add_ranges_test name)
  if(NOT BUILD_TESTING)
    return()
  endif()

  add_range_target(${name} ${ARGN})

  if(NOT TARGET ${name})
    return()
  endif()

  target_link_libraries(${name} Catch2::Catch2)
      
  include(Catch)
  catch_discover_tests(${name}
    PROPERTIES LABELS ${name})

  if(RUN_TESTS_POSTBUILD)
    add_custom_command(TARGET ${name}
      POST_BUILD
      COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -L ${name} --output-on-failure -j8
      COMMENT "Running ${name} tests")
  endif()

endfunction()

function(add_ranges_example name)
  add_range_target(${name} ${ARGN})
endfunction()

function(add_ranges_benchmark name)
  add_range_target(${name} range-v3 ${ARGN})
  target_link_libraries(${name} benchmark::benchmark)
endfunction()
