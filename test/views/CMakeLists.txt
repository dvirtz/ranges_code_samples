include(AddTarget)

add_ranges_test(views_range_v3 range-v3 main.cpp views.cpp range_v3.cpp)
add_ranges_test(views_stl2 cmcstl2 main.cpp views.cpp)
