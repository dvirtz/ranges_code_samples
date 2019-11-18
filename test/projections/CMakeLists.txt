include(AddTarget)

add_ranges_test(projections_range_v3 range-v3 main.cpp projections.cpp)
add_ranges_test(projections_stl2 stl2 main.cpp projections.cpp)
add_ranges_test(projections_nanorange "nanorange::nanorange" main.cpp projections.cpp)