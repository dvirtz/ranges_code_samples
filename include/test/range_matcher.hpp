#ifdef USE_RANGE_V3
#include <range/v3/algorithm/mismatch.hpp>
#include <range/v3/view/common.hpp>
#include <range/v3/view/ref.hpp>
#elif defined(USE_NANORANGE)
#include <nanorange.hpp>
namespace ranges = nano::ranges;
#else
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
namespace ranges = __stl2;
#endif
#include <utility/missing_utilities.hpp>
#include <sstream>
#include <catch2/catch.hpp>

namespace Catch {
  // namespace Detail {
  //   template <typename T>
  //   std::string stringify(const T& e);

  //   CPP_template(typename I, typename S)
  //   (requires (ranges::input_iterator<I> && ranges::sentinel_for<S, I>))
  //     std::string rangeToString(I first, S last) {
  //         std::stringstream rss;
  //         rss << "{ ";
  //         if (first != last) {
  //             rss << ::Catch::Detail::stringify(*first);
  //             for (++first; first != last; ++first)
  //                 rss << ", " << ::Catch::Detail::stringify(*first);
  //         }
  //         rss << " }";
  //         return rss.str();
  //     } 
  // }
  template <typename I, typename S, ranges::subrange_kind K> 
  struct is_range<ranges::subrange<I, S, K>> : std::false_type
  {};
  
  template <typename I, typename S, ranges::subrange_kind K> struct StringMaker<ranges::subrange<I, S, K>> {
    static std::string convert(ranges::subrange<I, S, K> view) {
      return rangeToString(view | ranges::views::common);
    }
  };
} // namespace Catch

template <typename LHS, typename RHS>
class RangeMatcher : public Catch::MatcherBase<LHS> {
public:
  RangeMatcher(RHS &&rhs) : m_rhs{std::forward<RHS>(rhs)} {}

  bool match(const LHS &lhs) const override { return match_impl(lhs); }

  template <typename R>
  bool CPP_fun(match_impl)(R lhs)(const requires ranges::range<R>) {
    using namespace ranges;
    using namespace std::string_literals;
    using Catch::Detail::stringify;

    if (distance(lhs) != distance(m_rhs)) {
      m_message = "of lengths mistmatch";
      return false;
    }

    if (auto [l, r] = mismatch(lhs); l != end(lhs)) {
      m_message = "at index "s + stringify(distance(begin(lhs), l)) + ": "
                  + stringify(*l) + " != " + stringify(*r);
      return false;
    }

    return true;
  }

  template <typename R>
  bool CPP_fun(match_impl)(const ranges::ref_view<R> &r)(
    const requires ranges::range<R> && (!std::is_array<R>::value)) {
    return match_impl(r.base());
  }

  template <typename L = LHS>
  auto CPP_fun(mismatch)(L &&lhs)(
    const requires std::is_floating_point_v<ranges::range_value_t<L>>) {
    return ranges::mismatch(std::forward<L>(lhs), m_rhs,
                            [](auto l, auto r) { return l == Approx(r); });
  }

  template <typename L = LHS>
  auto CPP_fun(mismatch)(L &&lhs)(
    const requires !std::is_floating_point_v<ranges::range_value_t<L>>) {
    return ranges::mismatch(std::forward<L>(lhs), m_rhs);
  }

  std::string describe() const override {
    using namespace ranges;
    auto common = m_rhs | views::common;
    return "not equal\n"
           + Catch::Detail::rangeToString(begin(common), end(common))
           + "\nbecause\n" + m_message;
  }

private:
  RHS m_rhs;
  mutable std::string m_message;
};

template <typename LHS, typename RHS> RangeMatcher<LHS, RHS> Equals(RHS &&rhs) {
  return {std::forward<RHS>(rhs)};
}

template <typename LHS, typename RHS>
void CPP_fun(check_equal)(const LHS &lhs, RHS &&rhs, bool /*force_call*/ = false)(
  requires ranges::forward_range<LHS> && ranges::forward_range<RHS> && ranges::range<const LHS>) {
  REQUIRE_THAT(lhs,
               Equals<decltype(lhs)>(std::forward<RHS>(rhs)));
}

template <typename LHS, typename RHS>
void CPP_fun(check_equal)(LHS &&lhs, RHS &&rhs, bool /*force_call*/ = false)(
  requires ranges::forward_range<LHS> &&ranges::forward_range<RHS>) {
  // intentionally not forwarding lhs to enforce it being an lvalue reference
  REQUIRE_THAT(ranges::subrange(lhs),
               Equals<decltype(ranges::subrange(lhs))>(std::forward<RHS>(rhs)));
}

template <typename LHS, typename RHS>
void CPP_fun(check_equal)(LHS &&lhs, RHS &&rhs, bool /*force_call*/ = false)(
  requires !ranges::forward_range<LHS> || !ranges::forward_range<RHS>) {
  using namespace ranges;
  REQUIRE_THAT(to_vector(std::forward<LHS>(lhs)),
               Equals<decltype(to_vector(std::forward<LHS>(lhs)))>(rhs));
}

template <typename LHS, typename T>
void check_equal(LHS &&lhs, std::initializer_list<T> rhs) {
  check_equal(std::forward<LHS>(lhs), rhs, true);
}