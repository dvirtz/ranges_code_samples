#include "test/range_matcher.hpp"
#ifdef USE_RANGE_V3
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view.hpp>
#elif defined USE_NANORANGE
#include <nanorange.hpp>
#endif
#include <catch2/catch.hpp>
#include <locale>
#include <sstream>

#ifdef USE_CMCSTL2
namespace std::experimental::ranges::v1::view {
using namespace ext;
}
#endif

using namespace ranges;

TEST_CASE("all") {
  const int rng[] = {1, 2, 3, 4};
  check_equal(views::all(rng), {1, 2, 3, 4});
}

TEST_CASE("commmon") {
  #ifdef USE_RANGE_V3
  auto unreachable_sentinel = unreachable;
  #endif
  
  auto &&rng = views::iota(0, unreachable_sentinel) | views::take(4);
  static_assert(!common_range<decltype(rng)>);
  auto &&res = views::common(rng);
  static_assert(common_range<decltype(res)>);
  REQUIRE(std::equal(res.begin(), res.end(), begin(rng)));
}

TEST_CASE("counted") {
  int rng[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::counted(begin(rng) + 2, 5), {3, 4, 5, 6, 7});
}

TEST_CASE("drop") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    check_equal(views::drop(views::all(rng), 6), {6, 7, 8, 9, 10});
  }

  SECTION("length > range size") {
    REQUIRE(empty(views::drop(views::all(rng), 12)));
  }
}

TEST_CASE("drop_while") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::drop_while(rng, [](int i) { return i < 6; }),
              {6, 7, 8, 9, 10});
}

TEST_CASE("empty") { REQUIRE(empty(views::empty<int>)); }

TEST_CASE("filter") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::filter(rng, [](int i) { return i % 2 == 0; }),
              {0, 2, 4, 6, 8, 10});
}

#if !defined(USE_STL2) && !defined(USE_NANORANGE)
TEST_CASE("generate") {
  // https://github.com/CaseyCarter/cmcstl2/issues/276
  auto &&res = views::generate([i = 0]() mutable { return i++; });
  check_equal(res | views::take(5), {0, 1, 2, 3, 4});
}
#endif

#ifndef USE_NANORANGE
TEST_CASE("indirect") {
  SECTION("RAW pointers") {
    const int arr[] = {0, 1, 2, 3, 4};
    auto &&rng      = views::iota(arr, arr + 5);
    check_equal(views::indirect(rng), arr);
  }

  SECTION("smart pointers") {
    auto rng =
      to_vector(views::iota(0, 5) | views::transform([](int i) mutable {
                  return std::make_shared<int>(i);
                }));
    auto &&res = views::indirect(rng);
    check_equal(res, {0, 1, 2, 3, 4});
  }
}
#endif

TEST_CASE("iota") {
  SECTION("numeric") { check_equal(views::iota(42, 45), {42, 43, 44}); }

  SECTION("non numeric") {
    const auto str = views::c_str("Hello Core C++");
    SECTION("lower and upper bounds") {
      auto &&res = views::iota(begin(str), end(str));
      check_equal(views::indirect(res), {'H', 'e', 'l', 'l', 'o', ' ', 'C', 'o',
                                         'r', 'e', ' ', 'C', '+', '+'});
    }

    SECTION("lower bound only") {
      auto &&res = views::iota(begin(str));
      check_equal(views::indirect(res) | views::take(10),
                  {'H', 'e', 'l', 'l', 'o', ' ', 'C', 'o', 'r', 'e'});
    }
  }
}

TEST_CASE("istream_range") {
  using namespace views;
  std::istringstream sst{"1 2 3 4 5 6"};
#ifdef USE_NANORANGE
  check_equal(istream_view<int>(sst), {1, 2, 3, 4, 5, 6});
#else
  check_equal(istream<int>(sst), {1, 2, 3, 4, 5, 6});
#endif
}

TEST_CASE("join") {
  const std::vector<std::vector<int>> rng{{0, 1, 2}, {3, 4, 5, 6}, {7, 8, 9}};
  check_equal(views::join(rng), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
}

#ifndef USE_NANORANGE
TEST_CASE("move") {
  std::vector<std::vector<int>> source{{0, 1, 2}, {3, 4, 5, 6}, {7, 8, 9}};
  std::vector<std::vector<int>> dest{3};
  copy(views::move(source), begin(dest));
  check_equal(
    dest, std::vector<std::vector<int>>{{0, 1, 2}, {3, 4, 5, 6}, {7, 8, 9}});
  check_equal(source, std::vector<std::vector<int>>{3});
}
#endif

#if !defined(USE_STL2) && !defined(USE_NANORANGE)
TEST_CASE("repeat") {
  // https://github.com/CaseyCarter/cmcstl2/issues/276
  check_equal(views::repeat(42) | views::take(6), {42, 42, 42, 42, 42, 42});
}

TEST_CASE("repeat_n") {
  // https://github.com/CaseyCarter/cmcstl2/issues/276
  check_equal(views::repeat_n(42, 6), {42, 42, 42, 42, 42, 42});
}
#endif

TEST_CASE("reverse") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6};
  check_equal(views::reverse(rng), {6, 5, 4, 3, 2, 1, 0});
}

TEST_CASE("single") { check_equal(views::single(42), {42}); }

TEST_CASE("split") {
  SECTION("str") {
    auto str = views::c_str(
      "One Proposal to ranges::merge them all, One Proposal to ranges::find them");

    SECTION("by value") {
      auto &&res    = views::split(str, ' ');
      auto splitted = {views::c_str("One"),  views::c_str("Proposal"),
                       views::c_str("to"),   views::c_str("ranges::merge"),
                       views::c_str("them"), views::c_str("all,"),
                       views::c_str("One"),  views::c_str("Proposal"),
                       views::c_str("to"),   views::c_str("ranges::find"),
                       views::c_str("them")};
      for (auto &&[actual, expected] : views::zip(res, splitted)) {
        check_equal(actual, expected);
      }
    }

    SECTION("by subrange") {
      auto &&res    = views::split(str, views::c_str("ranges::"));
      auto splitted = {views::c_str("One Proposal to "),
                       views::c_str("merge them all, One Proposal to "),
                       views::c_str("find them")};
      for (auto &&[actual, expected] : views::zip(res, splitted)) {
        check_equal(actual, expected);
      }
    }
  }

  SECTION("numbers") {
    const int rng[] = {0, 1, 2, 0, 1, 3, 0, 1, 4};
    SECTION("by value") {
      auto &&res = views::split(rng, 0);
      REQUIRE(distance(res) == 4);
      check_equal(*next(begin(res), 0), views::empty<int>);
      check_equal(*next(begin(res), 1), {1, 2});
      check_equal(*next(begin(res), 2), {1, 3});
      check_equal(*next(begin(res), 3), {1, 4});
    }

    SECTION("by subrange") {
      auto &&res = views::split(rng, views::iota(0, 2));
      REQUIRE(distance(res) == 4);
      check_equal(*next(begin(res), 0), views::empty<int>);
      check_equal(*next(begin(res), 1), {2});
      check_equal(*next(begin(res), 2), {3});
      check_equal(*next(begin(res), 3), {4});
    }
  }
}

TEST_CASE("subrange") {
  const int rng[] = {1, 2, 3, 4};

  SECTION("iterator pair") {
    check_equal(subrange{begin(rng) + 1, end(rng)}, {2, 3, 4});
  }

  SECTION("counted") {
    check_equal(subrange{begin(rng) + 1, end(rng) - 1, 2}, {2, 3});
  }
}

TEST_CASE("take") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    check_equal(views::take(rng, 6), {0, 1, 2, 3, 4, 5});
  }

  SECTION("length > range size") { check_equal(views::take(rng, 12), rng); }
}

#ifndef USE_NANORANGE
TEST_CASE("take_exactly") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    check_equal(views::take_exactly(rng, 6), {0, 1, 2, 3, 4, 5});
  }

  SECTION("length > range size") {
    // check_equal(views::take(rng, 12), rng); UB!!
    REQUIRE(size(views::take_exactly(rng, 12)) == 12);
  }
}
#endif

TEST_CASE("take_while") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::take_while(rng, [](int i) { return i < 6; }),
              {0, 1, 2, 3, 4, 5});
}

TEST_CASE("transform") {
  SECTION("unary") {
    using namespace std::string_literals;
    const int rng[] = {0, 1, 2, 3, 4, 5};
    check_equal(views::transform(rng, [](int i) { return std::to_string(i); }),
                {"0"s, "1"s, "2"s, "3"s, "4"s, "5"s});
  }
}
