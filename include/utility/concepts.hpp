#pragma once

#ifndef USE_RANGE_V3
#ifdef USE_STL2

#if (defined(__cpp_lib_type_trait_variable_templates) && \
    __cpp_lib_type_trait_variable_templates > 0)
#define META_CXX_TRAIT_VARIABLE_TEMPLATES 1
#else
#define META_CXX_TRAIT_VARIABLE_TEMPLATES 0
#endif

#if defined(__clang__)
#define META_IS_SAME(...) __is_same(__VA_ARGS__)
#elif defined(__GNUC__) && __GNUC__ >= 6
#define META_IS_SAME(...) __is_same_as(__VA_ARGS__)
#elif META_CXX_TRAIT_VARIABLE_TEMPLATES
#define META_IS_SAME(...) std::is_same_v<__VA_ARGS__>
#else
#define META_IS_SAME(...) std::is_same<__VA_ARGS__>::value
#endif

#if defined(__GNUC__) || defined(_MSC_VER)
#define META_IS_BASE_OF(...) __is_base_of(__VA_ARGS__)
#elif META_CXX_TRAIT_VARIABLE_TEMPLATES
#define META_IS_BASE_OF(...) std::is_base_of_v<__VA_ARGS__>
#else
#define META_IS_BASE_OF(...) std::is_base_of<__VA_ARGS__>::value
#endif

#if defined(__clang__) || defined(_MSC_VER) || \
    (defined(__GNUC__) && __GNUC__ >= 8)
#define META_IS_CONSTRUCTIBLE(...) __is_constructible(__VA_ARGS__)
#elif META_CXX_TRAIT_VARIABLE_TEMPLATES
#define META_IS_CONSTRUCTIBLE(...) std::is_constructible_v<__VA_ARGS__>
#else
#define META_IS_CONSTRUCTIBLE(...) std::is_constructible<__VA_ARGS__>::value
#endif

#endif

#include <concepts/concepts.hpp>

#endif