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

TEST_CASE("drop") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    auto &&res = actions::drop(rng, 6);
    check_equal(res, {6, 7, 8, 9, 10});
  }

  SECTION("length > range size") {
    auto &&res = actions::drop(rng, 12);
    REQUIRE(empty(res));
  }
}

TEST_CASE("drop_while") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto &&res = actions::drop_while(rng, [](int i) { return i < 6; });
  check_equal(res, {6, 7, 8, 9, 10});
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
                  "result is vector of value type of value type");
    REQUIRE(to<std::string>(res) == "Hello World");
  }
}

TEST_CASE("push_back") {
  std::vector rng{0, 1, 2};

  SECTION("value") {
    auto &&res = actions::push_back(rng, 42);
    check_equal(res, {0, 1, 2, 42});
    REQUIRE(&res == &rng);
  }

  SECTION("range") {
    auto &&res = actions::push_back(rng, views::indices(40, 42));
    check_equal(res, {0, 1, 2, 40, 41});
    REQUIRE(&res == &rng);
  }
}

TEST_CASE("push_front") {
  SECTION("list") {
    std::list rng{0, 1, 2};

    SECTION("value") {
      auto &&res = actions::push_front(rng, 42);
      check_equal(res, {42, 0, 1, 2});
      REQUIRE(&res == &rng);
    }

    SECTION("range") {
      auto &&res = actions::push_front(rng, views::indices(40, 42));
      check_equal(res, {40, 41, 0, 1, 2});
      REQUIRE(&res == &rng);
    }
  }

  SECTION("vector") {
    std::vector rng{0, 1, 2};

    SECTION("range") {
      auto &&res = actions::push_front(rng, views::indices(40, 42));
      check_equal(res, {40, 41, 0, 1, 2});
      REQUIRE(&res == &rng);
    }
  }
}

TEST_CASE("remove_if") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto &&res = actions::remove_if(rng, [](int i) { return i % 2 == 1; });
  REQUIRE(&res == &rng);
  check_equal(res, {0, 2, 4, 6, 8, 10});
}

TEST_CASE("reverse") {
  using namespace std::string_literals;
  std::vector rng{"Jeff"s, "Bill"s, "Warren"s, "Bernard"s, "Carlos"s};
  auto &&res = actions::reverse(rng);
  REQUIRE(&res == &rng);
  check_equal(res, {"Carlos"s, "Bernard"s, "Warren"s, "Bill"s, "Jeff"s});
}

TEST_CASE("shuffle") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  REQUIRE(!is_sorted(actions::shuffle(rng, std::mt19937{})));
}

TEST_CASE("slice") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6};

  SECTION("both from beginning") {
    auto &&res = actions::slice(rng, 2, 5);
    REQUIRE(&res == &rng);
    check_equal(res, {2, 3, 4});
  }

#if RANGE_V3_VERSION >= 10000
  SECTION("first from beginning, second from end") {
    auto &&res = actions::slice(rng, 2, end - 2);
    REQUIRE(&res == &rng);
    check_equal(res, {2, 3, 4});
  }

  SECTION("both from from") {
    auto &&res = actions::slice(rng, end - 5, end - 2);
    REQUIRE(&res == &rng);
    check_equal(res, {2, 3, 4});
  }
#endif
}

TEST_CASE("sort") {
  using namespace std::string_literals;
  std::vector rng{"Jeff"s, "Bill"s, "Warren"s, "Bernard"s, "Carlos"s};

  SECTION("default comparator") {
    check_equal(actions::sort(rng), {"Bernard"s, "Bill"s, "Carlos"s, "Jeff"s, "Warren"s});
  }

  SECTION("custom comparator") {
    auto &&res =
      actions::sort(rng, [](const std::string &lhs, const std::string &rhs) {
        return lexicographical_compare(views::reverse(lhs), views::reverse(rhs));
      });
    REQUIRE(&res == &rng);
    check_equal(res, {"Bernard"s, "Jeff"s, "Bill"s, "Warren"s, "Carlos"s});
  }
}

TEST_CASE("split") {
  using namespace std::string_literals;
  auto rng =
    "One Proposal to ranges::merge them all, One Proposal to ranges::find them"s;

  SECTION("by value") {
    auto &&res = actions::split(rng, ' ');
    check_equal(res,
               {"One"s, "Proposal"s, "to"s, "ranges::merge"s, "them"s, "all,"s,
                "One"s, "Proposal"s, "to"s, "ranges::find"s, "them"s});
  }

  SECTION("by subrange") {
    auto &&res = actions::split(rng, views::c_str("ranges::"));
    check_equal(res, {"One Proposal to "s, "merge them all, One Proposal to "s,
                     "find them"s});
  }
}

TEST_CASE("split_when") {
  using namespace std::string_literals;
  auto rng =
    "One Proposal to ranges::merge them all, One Proposal to ranges::find them"s;

  SECTION("by predicate") {
    auto &&res = actions::split_when(
      rng, [loc = std::locale{}](char c) { return std::ispunct(c, loc); });
    check_equal(res, {"One Proposal to ranges"s, "merge them all"s,
                     " One Proposal to ranges"s, "find them"s});
  }

  SECTION("by function") {
    auto &&res = actions::split_when(rng, [loc = std::locale{}](auto current, auto) {
      return std::pair{std::isupper(*current, loc), next(current)};
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
    return lexicographical_compare(s1, s2, ordered_less{}, toupper, toupper);
  };

  auto &&res = actions::stable_sort(rng, caseInsensitiveCompare);
  check_equal(res, {"Hello"s, "hElLo"s, "HELLO"s, "world"s, "WoRlD"s});
}

TEST_CASE("stride") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto &&res = actions::stride(rng, 3);
  check_equal(res, {0, 3, 6, 9});
}

TEST_CASE("take") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  SECTION("length < range size") {
    auto &&res = actions::take(rng, 6);
    check_equal(res, {0, 1, 2, 3, 4, 5});
  }

  SECTION("length > range size") {
    auto &&res = actions::take(rng, 12);
    check_equal(res, rng);
  }
}

TEST_CASE("take_while") {
  std::vector rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto &&res = actions::take_while(rng, [](int i) { return i < 6; });
  check_equal(res, {0, 1, 2, 3, 4, 5});
}

TEST_CASE("transform") {
  using namespace std::string_literals;
  std::vector rng{0, 1, 2, 3, 4, 5};
  auto &&res = actions::transform(rng, [](int i) { return i * i; });
  check_equal(res, {0, 1, 4, 9, 16, 25});
}

TEST_CASE("unique") {
  std::vector rng{1, 1, 1, 2, 5, 5, 5, 3};
  auto &&res = actions::unique(rng);
  check_equal(res, {1, 2, 5, 3});
}
