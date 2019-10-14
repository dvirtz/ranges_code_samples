include(AddTarget)

add_ranges_test(views_range_v3 range-v3 main.cpp views.cpp range_v3.cpp)
target_compile_options(views_range_v3 PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)
add_ranges_test(views_stl2 stl2 main.cpp views.cpp)
