#pragma once

#ifdef USE_RANGE_V3
#include <range/v3/view/c_str.hpp>

#ifdef _MSC_VER
namespace ranges::detail {
template <> struct is_char_type_<char8_t> : std::true_type {};
} // namespace ranges::detail
#endif // _MSC_VER

#else
////////////////////////////////////////////////////////////////////////////////
// CPP_fun
// Usage:
//   template <typename A, typename B>
//   void CPP_fun(foo)(A a, B b)([const]opt [noexcept(true)]opt
//       requires Concept1<A> && Concept2<B>)
//   {}
//
// Note: This macro cannot be used when the last function argument is a
//       parameter pack.
#define CPP_PP_EXPAND(...) __VA_ARGS__
#define CPP_FUN_IMPL_1_(...) (__VA_ARGS__) CPP_PP_EXPAND /**/
#define CPP_fun(X) X CPP_FUN_IMPL_1_

#ifdef USE_STL2
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
#include <vector>

STL2_OPEN_NAMESPACE {
#else
#include <nanorange.hpp>

NANO_BEGIN_NAMESPACE
#endif

#ifdef USE_STL2
  template <range R> using range_value_t = ext::range_value_t<R>;
#endif

  inline constexpr auto to_vector = [](auto &&rng) {
    using Cont  = std::vector<range_value_t<decltype(rng)>>;
    auto common = std::forward<decltype(rng)>(rng) | views::common;
    return Cont{begin(common), end(common)};
  };

  template <typename V>
  auto CPP_fun(operator|)(V && v, decltype(to_vector) tv)(requires view<V>) {
    return tv(std::forward<V>(v));
  }

  namespace views {

  namespace detail {
  struct c_str_fn {
    template <typename Char, size_t N> auto operator()(Char (&sz)[N]) const {
      return subrange{&sz[0], &sz[N - 1]};
    }
  };

  } // namespace detail

  inline constexpr detail::c_str_fn c_str;

#ifdef USE_NANORANGE
  inline constexpr auto indirect = [](auto &&rng) {
    return transform(rng, [](auto &&p) { return *p; });
  };
#endif

  inline constexpr auto zip = [](auto &&rng1, auto &&rng2) {
    std::vector<
      std::pair<range_value_t<decltype(rng1)>, range_value_t<decltype(rng2)>>>
      v;
    ranges::transform(std::forward<decltype(rng1)>(rng1),
                      std::forward<decltype(rng2)>(rng2),
                      ranges::back_inserter(v), [](auto &&val1, auto &&val2) {
                        return std::pair{std::forward<decltype(val1)>(val1),
                                         std::forward<decltype(val2)>(val2)};
                      });
    return v;
  };

#ifdef USE_STL2
  using namespace ext;
#endif

  } // namespace views

#ifdef USE_NANORANGE
  template<typename V>
  std::ostream& CPP_fun(operator<<)(std::ostream &ost, V v)(requires view<V>){
    return ost;
  }
#endif

#ifdef USE_STL2
}
STL2_CLOSE_NAMESPACE
#else
NANO_END_NAMESPACE
#endif // USE_STL2

#endif // USE_RANGE_V3

// namespace fmt {}
