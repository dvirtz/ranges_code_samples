include(AddTarget)

add_ranges_test(algorithms_range_v3 range-v3 main.cpp algorithms.cpp numeric.cpp)
add_ranges_test(algorithms_stl2 stl2 main.cpp algorithms.cpp)
add_ranges_test(algorithms_nanorange "nanorange::nanorange" main.cpp algorithms.cpp)