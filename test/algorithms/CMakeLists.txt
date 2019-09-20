include(AddTarget)

add_ranges_test(algorithms_range_v3 range-v3 main.cpp algorithms.cpp range_v3.cpp)
add_ranges_test(algorithms_stl2 cmcstl2 main.cpp algorithms.cpp cmcstl2.cpp)