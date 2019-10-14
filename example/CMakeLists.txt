include(AddTarget)


add_ranges_example(game range-v3 game.cpp)
add_ranges_example(game_v2 range-v3 game_v2.cpp)
foreach(example game game_v2)
    target_compile_options(${example} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
endforeach()
add_ranges_example(cmcstl2_example stl2 cmcstl2.cpp)