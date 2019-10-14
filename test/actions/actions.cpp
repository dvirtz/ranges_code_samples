#include "test/range_matcher.hpp"
#include <catch2/catch.hpp>
#include <list>
#include <random>
#include <range/v3/action.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/view/c_str.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/reverse.hpp>
#include <set>

using namespace ranges;

TEST_CASE("pipe") {
  auto read_data = []()->std::vector<int> { return {}; };

  // extern std::vector<int> read_data();

  auto vi = read_data()
          | actions::sort
          | actions::unique;

  static_assert(std::is_same_v<decltype(vi), std::vector<int>>);
}

TEST_CASE("drop") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    check_equal(actions::drop(rng, 6), {6, 7, 8, 9, 10});
  }

  SECTION("length > range size") {
    REQUIRE(empty(actions::drop(rng, 12)));
  }
}

TEST_CASE("drop_while") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check_equal(actions::drop_while(rng, [](int i) { return i < 6; }), {6, 7, 8, 9, 10});
}

TEST_CASE("erase") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // SECTION("iterator") {
  //   auto &&res = actions::erase(rng, begin(rng));
  //   REQUIRE(*res == 1);
  //   check_equal(rng, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  // }

  SECTION("iterator range") {
    auto &&res = actions::erase(rng, begin(rng), next(begin(rng), 2));
    REQUIRE(*res == 2);
    check_equal(rng, {2, 3, 4, 5, 6, 7, 8, 9, 10});
  }
}

TEST_CASE("insert") {
  SECTION("set") {
    std::set<int> rng;

    SECTION("value") {
      auto &&[it, inserted] = actions::insert(rng, 42);
      REQUIRE(inserted);
      REQUIRE(*it == 42);
      check_equal(rng, {42});
    }

    SECTION("range") {
      actions::insert(rng, views::indices(5));
      check_equal(rng, {0, 1, 2, 3, 4});
    }
  }

  SECTION("vector") {
    std::vector rng{0, 1, 2};

    SECTION("value") {
      auto &&res = actions::insert(rng, begin(rng), 42);
      REQUIRE(*res == 42);
      check_equal(rng, {42, 0, 1, 2});
    }

    SECTION("count") {
      auto &&res = actions::insert(rng, end(rng), 3, 42);
      REQUIRE(*res == 42);
      check_equal(rng, {0, 1, 2, 42, 42, 42});
    }

    SECTION("range") {
      auto &&res =
        actions::insert(rng, next(begin(rng), 2), views::indices(40, 42));
      REQUIRE(*res == 40);
      check_equal(rng, {0, 1, 40, 41, 2});
    }
  }
}

TEST_CASE("join") {
  using namespace std::string_literals;

  SECTION("value is a container") {
    std::vector text{"Hello"s, " "s, "World"s};
    auto res = actions::join(text);
    static_assert(std::is_same_v<decltype(res), std::string>,
                  "result is value type");
    REQUIRE(res == "Hello World");
  }

  SECTION("value is not a container") {
    std::vector text{views::c_str("Hello"), views::c_str(" "),
                     views::c_str("World")};
    auto res = actions::join(text);
    static_assert(std::is_same_v<decltype(res), std::vector<char>>,
                  "result is vector of value type");
    REQUIRE(to<std::string>(res) == "Hello World");
  }
}

TEST_CASE("push_back") {
  std::vector rng{0, 1, 2};

  SECTION("value") {
    check_equal(actions::push_back(rng, 42), {0, 1, 2, 42});
  }

  SECTION("range") {
    check_equal(actions::push_back(rng, views::indices(40, 42)), {0, 1, 2, 40, 41});
  }
}

TEST_CASE("push_front") {
  SECTION("list") {
    std::list rng{0, 1, 2};

    SECTION("value") {
      check_equal(actions::push_front(rng, 42), {42, 0, 1, 2});
    }

    SECTION("range") {
      check_equal(actions::push_front(rng, views::indices(40, 42)), {40, 41, 0, 1, 2});
    }
  }

  SECTION("vector") {
    std::vector rng{0, 1, 2};

    SECTION("range") {
      actions::push_front(rng, views::indices(40, 42));
      check_equal(rng, {40, 41, 0, 1, 2});
    }
  }
}

TEST_CASE("remove_if") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  actions::remove_if(rng, [](int i) { return i % 2 == 1; });
  check_equal(rng, {0, 2, 4, 6, 8, 10});
}

TEST_CASE("reverse") {
  using namespace std::string_literals;
  std::vector rng{"Jeff"s, "Bill"s, "Warren"s, "Bernard"s, "Carlos"s};
  actions::reverse(rng);
  check_equal(rng, {"Carlos"s, "Bernard"s, "Warren"s, "Bill"s, "Jeff"s});
}

TEST_CASE("shuffle") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  REQUIRE(!is_sorted(actions::shuffle(rng, std::mt19937{})));
}

TEST_CASE("slice") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6};

  SECTION("both from beginning") {
    actions::slice(rng, 2, 5);
    check_equal(rng, {2, 3, 4});
  }

  SECTION("first from beginning, second from end") {
    actions::slice(rng, 2, end - 2);
    check_equal(rng, {2, 3, 4});
  }

  SECTION("both from from") {
    actions::slice(rng, end - 5, end - 2);
    check_equal(rng, {2, 3, 4});
  }
}

TEST_CASE("sort") {
  using namespace std::string_literals;
  std::vector rng{"Jeff"s, "Bill"s, "Warren"s, "Bernard"s, "Carlos"s};

  SECTION("default comparator") {
    check_equal(actions::sort(rng), {"Bernard"s, "Bill"s, "Carlos"s, "Jeff"s, "Warren"s});
  }

  SECTION("custom comparator") {
      actions::sort(rng, [](const std::string &lhs, const std::string &rhs) {
        return lexicographical_compare(views::reverse(lhs), views::reverse(rhs));
      });
    check_equal(rng, {"Bernard"s, "Jeff"s, "Bill"s, "Warren"s, "Carlos"s});
  }
}

TEST_CASE("split") {
  using namespace std::string_literals;
  auto rng =
    "One Proposal to ranges::merge them all, One Proposal to ranges::find them"s;

  SECTION("by value") {
    auto&& res = actions::split(rng, ' ');
    check_equal(res,
               {"One"s, "Proposal"s, "to"s, "ranges::merge"s, "them"s, "all,"s,
                "One"s, "Proposal"s, "to"s, "ranges::find"s, "them"s});
  }

  SECTION("by subrange") {
    auto&& res = actions::split(rng, views::c_str("ranges::"));
    check_equal(res, {"One Proposal to "s, "merge them all, One Proposal to "s,
                     "find them"s});
  }
}

TEST_CASE("split_when") {
  using namespace std::string_literals;
  auto rng =
    "One Proposal to ranges::merge them all, One Proposal to ranges::find them"s;

  SECTION("by predicate") {
    auto&& res = actions::split_when(
      rng, [](char c) { return std::ispunct(c); });
    check_equal(res, {"One Proposal to ranges"s, "merge them all"s,
                     " One Proposal to ranges"s, "find them"s});
  }

  SECTION("by function") {
    auto&& res = actions::split_when(rng, [](auto current, auto) {
      return std::pair{std::isupper(*current), next(current)};
    });
    check_equal(res, {""s, "ne "s, "roposal to ranges::merge them all, "s,
                     "ne "s, "roposal to ranges::find them"s});
  }
}

TEST_CASE("stable_sort") {
  using namespace std::string_literals;
  std::vector rng{"world"s, "Hello"s, "hElLo"s, "WoRlD"s, "HELLO"s};

  auto const caseInsensitiveCompare = [](const std::string &s1,
                                         const std::string &s2) {
    const auto toupper = [](char c) {
      return static_cast<char>(std::toupper(c));
    };
    return lexicographical_compare(s1, s2, less{}, toupper, toupper);
  };

  actions::stable_sort(rng, caseInsensitiveCompare);
  check_equal(rng, {"Hello"s, "hElLo"s, "HELLO"s, "world"s, "WoRlD"s});
}

TEST_CASE("stride") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  actions::stride(rng, 3);
  check_equal(rng, {0, 3, 6, 9});
}

TEST_CASE("take") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    actions::take(rng, 6);
    check_equal(rng, {0, 1, 2, 3, 4, 5});
  }

  SECTION("length > range size") {
    actions::take(rng, 12);
    check_equal(rng, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  }
}

TEST_CASE("take_while") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  actions::take_while(rng, [](int i) { return i < 6; });
  check_equal(rng, {0, 1, 2, 3, 4, 5});
}

TEST_CASE("transform") {
  using namespace std::string_literals;
  std::vector rng{0, 1, 2, 3, 4, 5};
  actions::transform(rng, [](int i) { return i * i; });
  check_equal(rng, {0, 1, 4, 9, 16, 25});
}

TEST_CASE("unique") {
  std::vector rng{1, 1, 1, 2, 5, 5, 5, 3};
  actions::unique(rng);
  check_equal(rng, {1, 2, 5, 3});
}
