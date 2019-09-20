#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
#include "test/range_matcher.hpp"
#include <catch2/catch.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view.hpp>
#include <map>
#include <sstream>
#include <range/v3/to_container.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/functional/arithmetic.hpp>

using namespace ranges;

TEST_CASE("addressof") {
  const int rng[] = {1, 2, 3, 4};
  check_equal(views::addressof(rng), {rng, rng + 1, rng + 2, rng + 3});
}

TEST_CASE("adjacent_filter") {
  const int rng[] = {1, 1, 1, 2, 5, 5, 5, 3};
  check_equal(views::adjacent_filter(rng, ranges::not_equal_to{}), {1, 2, 5, 3});
}

TEST_CASE("adjacent_remove_if") {
  const int rng[] = {1, 1, 1, 2, 5, 5, 5, 3};
  check_equal(views::adjacent_remove_if(rng, ranges::equal_to{}), {1, 2, 5, 3});
}

TEST_CASE("any_view") {
    any_view<int, category::forward> any;
    REQUIRE(empty(any));
    any = views::iota(0, 4);
    any = views::single(42);
    check_equal(any, {42});
    // any = views::generate_n([](){ return 42; }, 4); does not compile
}

TEST_CASE("c_str") {
  SECTION("literal") {
    check_equal(views::c_str("Core C++"), {'C', 'o', 'r', 'e', ' ', 'C', '+', '+'});
    check_equal(views::c_str("cppcon"), {'c', 'p', 'p', 'c', 'o', 'n'});
  }

  SECTION("pointer") {
    check_equal(views::c_str(static_cast<const char *>("hello")), {'h', 'e', 'l', 'l', 'o'});
  }
}

TEST_CASE("cartesian_product") {
  using std::tuple;
  using namespace std::string_literals;
  const int numbers[]       = {1, 2, 3};
  std::string const names[] = {"eric"s, "casey"s};
  check_equal(views::cartesian_product(numbers, names), {tuple{1, "eric"s}, tuple{1, "casey"s}, tuple{2, "eric"s},
                   tuple{2, "casey"s}, tuple{3, "eric"s}, tuple{3, "casey"s}});
}

TEST_CASE("chunk") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto &&res      = views::chunk(rng, 3);
  REQUIRE(size(res) == 4);
  check_equal(res[0], {0, 1, 2});
  check_equal(res[1], {3, 4, 5});
  check_equal(res[2], {6, 7, 8});
  check_equal(res[3], {9});
}

TEST_CASE("inclusive_indices") {
  SECTION("lower and upper bounds") {
    auto &&res = views::closed_indices(0, 4);
    check_equal(res, {0, 1, 2, 3, 4});
  }

  SECTION("upper bound only") {
    auto &&res = views::closed_indices(3);
    check_equal(res, {0, 1, 2, 3});
  }
}

TEST_CASE("concat") {
  const int rng0[] = {1, 2, 3}, rng1[] = {4, 5, 6}, rng2[] = {7, 8};
  check_equal(views::concat(rng0, rng1, rng2), {1, 2, 3, 4, 5, 6, 7, 8});
}

TEST_CASE("const_") {
  int rng[]  = {1, 2, 3, 4};
  auto &&res = views::const_(rng);
  static_assert(std::is_same_v<range_reference_t<decltype(rng)>, int &>,
                "default should be non const");
  static_assert(std::is_same_v<range_reference_t<decltype(res)>, const int &>,
                "const_ should be const");
  check_equal(res, {1, 2, 3, 4});
}

TEST_CASE("cycle") {
  const int rng[] = {0, 1, 2};
  check_equal(views::take(views::cycle(rng), 10), {0, 1, 2, 0, 1, 2, 0, 1, 2, 0});
}

TEST_CASE("delimit") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  SECTION("value not found") {
#if RANGE_V3_VERSION < 10000
    auto &&res = views::delimit(views::all(rng), 42);
#else
    // https://github.com/ericniebler/range-v3/pull/1073
    auto &&res = views::delimit(rng, 42);
#endif
    check_equal(res, rng);
  }

  SECTION("value found") {
    check_equal(views::delimit(rng, 5), {0, 1, 2, 3, 4});
  }

  SECTION("iterator") {
    auto &&res = views::delimit(begin(rng) + 2, 6);
    check_equal(res, {2, 3, 4, 5});
  }
}

TEST_CASE("drop_exactly") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    auto &&res = views::drop_exactly(views::all(rng), 6);
    check_equal(res, {6, 7, 8, 9, 10});
  }

  SECTION("length > range size") {
    REQUIRE(begin(views::drop_exactly(views::all(rng), 12)) > end(rng));
    // views::drop_exactly(views::all(rng), 12); UB!!
  }
}

TEST_CASE("enumerate") {
  using namespace std::string_literals;
  using std::pair;
  std::string const names[] = {"adi"s, "michael"s, "eran"s, "amir"s};
  // check_equal(views::enumerate(names), {pair{0, "adi"s}, pair{1, "michael"s}, pair{2, "eran"s},
  //                  pair{3, "amir"s}});
}

TEST_CASE("exclusive_scan") {
  const int rng[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("default method") {
    check_equal(views::exclusive_scan(rng, 0), {0, 1, 3, 6, 10, 15, 21, 28, 36, 45});
  }

  SECTION("custom method") {
    check_equal(views::exclusive_scan(rng, 1, ranges::multiplies{}), {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880});
  }
}

TEST_CASE("for_each") {
  const int rng[] = {0, 1, 2, 3};

  SECTION("yield") {
    check_equal(views::for_each(rng, [](int i) { return yield(i * i); }), {0, 1, 4, 9});
  }

  SECTION("yield_from") {
    check_equal(views::for_each(rng, [](int i) { return yield_from(views::indices(i)); }), {0, 0, 1, 0, 1, 2});
  }

  SECTION("yield_if") {
    check_equal(views::for_each(rng, [](int i) { return yield_if(i % 2 == 0, i / 2); }), {0, 1});
  }

  SECTION("lazy_yield_if") {
    check_equal(views::for_each(rng, [](int i) {
      return lazy_yield_if(i % 2 == 0, [i] { return i / 2; });
    }), {0, 1});
  }
}

TEST_CASE("generate_n") {
  check_equal(views::generate_n([i = 0]() mutable { return i++; }, 5), 
  {0, 1, 2, 3, 4});
}

TEST_CASE("getlines") {
  std::istringstream sst{
    R"(One Proposal to
ranges::merge them all,
One Proposal to
ranges::find them)"};
  check_equal(getlines(sst), 
  {"One Proposal to", "ranges::merge them all,",
  "One Proposal to", "ranges::find them"});
}

TEST_CASE("group_by") {
  const int rng[] = {1, 1, 2, 2, 1, 3};
  auto &&res      = views::group_by(rng, ranges::equal_to{});
  REQUIRE(distance(res) == 4);
  check_equal(*next(begin(res), 0), {1, 1});
  check_equal(*next(begin(res), 1), {2, 2});
  check_equal(*next(begin(res), 2), {1});
  check_equal(*next(begin(res), 3), {3});
}

TEST_CASE("indices") {
  SECTION("lower and upper bounds") {
    auto &&res = views::indices(0, 4);
    check_equal(res, {0, 1, 2, 3});
  }

  SECTION("upper bound only") {
    auto &&res = views::indices(3);
    check_equal(res, {0, 1, 2});
  }
}

TEST_CASE("intersperse") {
  const int rng[] = {0, 1, 2, 3, 4};
  check_equal(views::intersperse(rng, 42), {0, 42, 1, 42, 2, 42, 3, 42, 4});
}

TEST_CASE("ints") {
  SECTION("lower and upper bounds") {
    auto &&res = views::ints(3, 7);
    check_equal(res, {3, 4, 5, 6});
  }

  SECTION("lower bound only") {
    auto &&res = views::ints(3, unreachable);
    check_equal(res | views::take(4), {3, 4, 5, 6});
  }
}

TEST_CASE("closed_iota") {
    check_equal(views::closed_iota(42, 45), {42, 43, 44, 45});
}

TEST_CASE("join extra") {
  const std::vector<std::vector<int>> rng{{0, 1, 2}, {3, 4, 5, 6}, {7, 8, 9}};

  SECTION("with value") {
    check_equal(views::join(rng, 42), {0, 1, 2, 42, 3, 4, 5, 6, 42, 7, 8, 9});
  }

  SECTION("with range") {
    check_equal(views::join(rng, views::iota(42, 44)), {0, 1, 2, 42, 43, 3, 4, 5, 6, 42, 43, 7, 8, 9});
  }
}

TEST_CASE("elements") {
  using namespace std::string_literals;
  std::map<int, std::string> m{{1, "one"}, {2, "two"}, {3, "three"}};

  SECTION("keys") {
  check_equal(views::keys(m), {1, 2, 3});
  }

  SECTION("values") {
  check_equal(views::values(m), {"one"s, "two"s, "three"s});
  }
}

TEST_CASE("linear_distribute") {
  check_equal(views::linear_distribute(0.5, 1.4, 10), 
  {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4});
}

TEST_CASE("partial_sum") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6};

  SECTION("default") {
    auto &&res = views::partial_sum(rng, plus{});
    check_equal(res, {0, 1, 3, 6, 10, 15, 21});
  }

  SECTION("custom") {
    auto &&res =
      views::partial_sum(rng, [prev = 0](int current, int next) mutable {
        auto res = current + prev;
        prev += next;
        return res;
      });
    check_equal(res, {0, 0, 1, 4, 10, 20, 35});
  }
}

TEST_CASE("remove_if") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::remove_if(rng, [](int i) { return i % 2 == 0; }), {1, 3, 5, 7, 9});
}

TEST_CASE("replace") {
  const int rng[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  check_equal(views::replace(rng, 1, 42), {42, 2, 3, 42, 2, 3, 42, 2, 3});
}

TEST_CASE("replace_if") {
  const int rng[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  check_equal(views::replace_if(rng, [](int i) { return i != 1; }, 42), {1, 42, 42, 1, 42, 42, 1, 42, 42});
}

TEST_CASE("sample") {
  auto rng   = views::closed_iota('a', 'z');
  REQUIRE(distance(views::sample(rng, 5)) == 5);
  REQUIRE(is_sorted(views::sample(rng, 5) | to_vector));
}

TEST_CASE("set_views") {
  auto multiples_of_3 = views::stride(views::ints, 3);
  auto squares = views::transform(views::ints, [](int x) { return x * x; });

  SECTION("set_difference") {
    check_equal(views::take(views::set_difference(multiples_of_3, squares), 6), {3, 6, 12, 15, 18, 21});
  }

  SECTION("set_intersection") {
    check_equal(views::take(views::set_intersection(multiples_of_3, squares), 6), {0, 9, 36, 81, 144, 225});
  }

  SECTION("set_union") {
    check_equal(views::take(views::set_union(multiples_of_3, squares), 6), {0, 1, 3, 4, 6, 9});
  }

  SECTION("set_symmetric_difference") {
    check_equal(views::take(views::set_symmetric_difference(multiples_of_3, squares), 6), {1, 3, 4, 6, 12, 15});
  }
}

TEST_CASE("slice") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6};

  SECTION("both from beginning") {
    check_equal(views::slice(rng, 2, 5), {2, 3, 4});
  }

  SECTION("first from beginning, second from end") {
    check_equal(views::slice(rng, 2, end - 2), {2, 3, 4});
  }

  SECTION("both from from") {
    check_equal(views::slice(rng, end - 5, end - 2), {2, 3, 4});
  }
}

TEST_CASE("sliding") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6};
  auto &&res = views::sliding(rng, 3);
  REQUIRE(distance(res) == 5);
  for (auto &&i : views::indices(5)) {
    check_equal(res[i], {i, i + 1, i + 2});
  }
}

TEST_CASE("split_when") {
  SECTION("str") {
    auto rng = views::c_str(
      "One Proposal to ranges::merge them all, One Proposal to ranges::find them");

    SECTION("by predicate") {
      auto &&res = views::split_when(
        rng, [loc = std::locale{}](char c) { return std::ispunct(c, loc); });
      auto splitted = {
        views::c_str("One Proposal to ranges"), views::c_str("merge them all"),
        views::c_str(" One Proposal to ranges"), views::c_str("find them")};
      for (auto &&[actual, expected] : views::zip(res, splitted)) {
        check_equal(actual, expected);
      }
    }

    SECTION("by function") {
      auto &&res =
        views::split_when(rng, [loc = std::locale{}](auto current, auto) {
          return std::pair{std::isupper(*current, loc), next(current)};
        });
      auto splitted = {views::c_str(""), views::c_str("ne "),
                       views::c_str("roposal to ranges::merge them all, "),
                       views::c_str("ne "),
                       views::c_str("roposal to ranges::find them")};
      for (auto &&[actual, expected] : views::zip(res, splitted)) {
        check_equal(actual, expected);
      }
    }
  }

  SECTION("numbers") {
    const int rng[] = {0, 1, 2, 0, 1, 3, 0, 1, 4};
    SECTION("by predicate") {
      auto &&res = views::split_when(rng, [](int i) { return i % 3 == 2; });
      REQUIRE(distance(res) == 2);
      check_equal(*next(begin(res), 0), {0, 1});
      check_equal(*next(begin(res), 1), {0, 1, 3, 0, 1, 4});
    }

    SECTION("by function") {
      auto &&res = views::split_when(rng, [](auto first, auto last) {
        return std::pair{*first == 0, next(first, 2, last)};
      });
      REQUIRE(distance(res) == 4);
      check_equal(*next(begin(res), 0), views::empty<int>);
      check_equal(*next(begin(res), 1), {2});
      check_equal(*next(begin(res), 2), {3});
      check_equal(*next(begin(res), 3), {4});
    }
  }
}

TEST_CASE("stride") {
  const int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(views::stride(rng, 3), {0, 3, 6, 9});
}

TEST_CASE("tail") {
  SECTION("empty source") {
    auto rng = views::empty<int>;
#if RANGE_V3_VERSION < 400
    auto &&res = rng;
#else
    // https://github.com/ericniebler/range-v3/issues/743
    auto &&res = views::tail(rng);
#endif
    REQUIRE(empty(res));
  }

  SECTION("single source") {
    auto rng   = views::single(1);
    auto &&res = views::tail(rng);
    REQUIRE(empty(res));
  }

  SECTION("non empty tail") {
    const int rng[] = {0, 1, 2, 3, 4, 5};
    auto &&res      = views::tail(rng);
    check_equal(res, {1, 2, 3, 4, 5});
  }
}

TEST_CASE("tokenize") {
  using namespace std::string_literals;

  auto rng =
    "One Proposal to ranges::merge them all, One Proposal to ranges::find them"s;

  SECTION("default") {
    auto &&res = views::tokenize(rng, std::regex{"\\s"});
    check_equal(res, views::repeat_n(" "s, 10));
  }

  SECTION("1") {
    auto str = "abc\ndef\tghi klm"s; 
    check_equal(views::tokenize(str, std::regex{"([a-z]+)"}, 1), 
    {"abc"s, "def"s, "ghi"s, "klm"s});
  }

  SECTION("-1") {
    auto &&res = views::tokenize(rng, std::regex{"\\s"}, -1);
    check_equal(res,
               {"One"s, "Proposal"s, "to"s, "ranges::merge"s, "them"s, "all,"s,
                "One"s, "Proposal"s, "to"s, "ranges::find"s, "them"s});
  }
}

TEST_CASE("transform extra") {
  SECTION("binary") {
    const int rng0[] = {0, 1, 2, 3, 4, 5};
    const int rng1[] = {6, 7, 8, 9, 10, 11};
    auto &&res       = views::transform(rng0, rng1, plus{});
    check_equal(res, {6, 8, 10, 12, 14, 16});
  }
}

TEST_CASE("unbounded") {
  int *p     = nullptr;
  auto &&res = views::delimit(p, 42);
}

TEST_CASE("unique") {
  const int rng[] = {1, 1, 1, 2, 5, 5, 5, 3};
  check_equal(views::unique(rng), {1, 2, 5, 3});
}

TEST_CASE("zip") {
  using std::pair;
  using namespace std::string_literals;
  const std::string names[] = {"john", "paul", "george", "richard"};
  const int songs[]         = {72, 70, 22, 2};
  check_equal(views::zip(names, songs), {pair{"john"s, 72}, pair{"paul"s, 70}, pair{"george"s, 22},
                   pair{"richard"s, 2}});
}

TEST_CASE("zip_with") {
  using namespace std::string_literals;
  const std::string names[] = {"john", "paul", "george", "richard"};
  const int songs[]         = {72, 70, 22, 2};
  check_equal(views::zip_with([](const std::string &name, const int songs){
    return name + " wrote " + std::to_string(songs);
  }, names, songs), 
  {"john wrote 72"s, "paul wrote 70"s, "george wrote 22"s,
                   "richard wrote 2"s});
}

TEST_CASE("to_container") {
  SECTION("vector") {
    auto vec = views::ints | views::transform([](int i) { return i + 42; })
               | views::take(10) | to<std::vector>;
    static_assert(std::is_same_v<decltype(vec), std::vector<int>>);
  }

  SECTION("map") {
    const std::string names[] = {"john", "paul", "george", "richard"};
    const int songs[]         = {72, 70, 22, 2};
    auto map = views::zip(names, songs) | to<std::map<std::string, int>>;
    check_equal(map,
               std::initializer_list<std::pair<const std::string, int>>{
                 {"george", 22}, {"john", 72}, {"paul", 70}, {"richard", 2}});
  }
}