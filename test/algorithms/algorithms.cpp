#include <catch2/catch.hpp>
#ifdef USE_RANGE_V3
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#else
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
#endif
#include "test/range_matcher.hpp"
#include <vector>

using namespace ranges;

TEST_CASE("dangling") {
  const auto f = []() -> std::vector<int> { return {}; };

  auto result1 = find(f(), 42);
  static_assert(same_as<decltype(result1), dangling>);
  
  auto vec     = f();
  auto result2 = find(vec, 42);
  static_assert(same_as<decltype(result2), std::vector<int>::iterator>);

  auto result3 = find(views::all(vec), 42);
  static_assert(same_as<decltype(result3), std::vector<int>::iterator>);
}

TEST_CASE("adjacent_find") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("default predicate") {
    auto &&res = adjacent_find(rng);
    REQUIRE(res == begin(rng) + 2);
  }

  SECTION("custom predicate") {
    auto &&res = adjacent_find(rng, [](int a, int b) { return a + b == 5; });
    REQUIRE(res == begin(rng) + 1);
  }
}

TEST_CASE("all_of") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("holds") {
    auto &&res = all_of(rng, [](int i) { return i < 5; });
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res = all_of(rng, [](int i) { return i == 5; });
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("any_of") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("holds") {
    auto &&res = any_of(rng, [](int i) { return i == 3; });
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res = any_of(rng, [](int i) { return i == 5; });
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("binary_search") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("is found") {
    auto &&res = binary_search(rng, 2);
    REQUIRE(res);
  }

  SECTION("is not found") {
    auto &&res = binary_search(rng, 5);
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("copy") {
  const int rng[] = {1, 2, 3, 3, 4};
  int out[size(rng)];
  copy(rng, out);
  check_equal(out, rng);
}

TEST_CASE("copy_backward") {
  int rng[] = {1, 2, 3, 3, 4};
  copy_backward(begin(rng), begin(rng) + 3, end(rng));
  check_equal(rng, {1, 2, 1, 2, 3});
}

TEST_CASE("copy_if") {
  const int rng[] = {1, 2, 3, 3, 4};
  std::vector<int> out;
  copy_if(rng, ranges::back_inserter(out), [](int i) { return i % 2 == 0; });
  check_equal(out, {2, 4});
}

TEST_CASE("copy_n") {
  const int rng[] = {1, 2, 3, 3, 4};
  std::vector<int> out;
  copy_n(begin(rng) + 1, 2, ranges::back_inserter(out));
  check_equal(out, {2, 3});
}

TEST_CASE("count") {
  const int rng[] = {1, 2, 3, 3, 4};
  auto &&res      = count(rng, 3);
  REQUIRE(res == 2);
}

TEST_CASE("count_if") {
  const int rng[] = {1, 2, 3, 3, 4};
  auto &&res      = count_if(rng, [](int i) { return i < 3; });
  REQUIRE(res == 2);
}

TEST_CASE("equal") {
  SECTION("equals") {
    const int rng0[] = {1, 2, 3, 3, 4};
    const int rng1[] = {1, 2, 3, 3, 4};
    auto &&res       = equal(rng0, rng1);
    REQUIRE(res);
  }

  SECTION("not equals") {
    const int rng0[] = {1, 2, 3, 3, 4};
    const int rng1[] = {1, 2, 3, 3, 5};
    auto &&res       = equal(rng0, rng1);
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("equal_range") {
  const int rng[] = {1, 2, 3, 3, 5};
  SECTION("found") {
    auto &&res = equal_range(rng, 3);
    REQUIRE(begin(res) == begin(rng) + 2);
    REQUIRE(end(res) == begin(rng) + 4);
  }

  SECTION("not found") {
    auto &&res = equal_range(rng, 4);
    REQUIRE(begin(res) == begin(rng) + 4);
    REQUIRE(end(res) == begin(rng) + 4);
  }
}

TEST_CASE("fill") {
  int out[5];
  fill(out, 42);
  check_equal(out, {42, 42, 42, 42, 42});
}

TEST_CASE("fill_n") {
  std::vector<int> out;
  fill_n(back_inserter(out), 5, 42);
  check_equal(out, {42, 42, 42, 42, 42});
}

TEST_CASE("find") {
  const int rng[] = {4, 2, 3, 1, 4};

  SECTION("found") {
    auto &&res = find(rng, 2);
    REQUIRE(res == begin(rng) + 1);
  }

  SECTION("not found") {
    auto &&res = find(rng, 5);
    REQUIRE(res == end(rng));
  }
}

TEST_CASE("find_end") {
  const int rng[] = {1, 2, 3, 4, 3, 4};

  SECTION("found") {
    const int to_find[] = {3, 4};
#ifdef USE_RANGE_V3
    auto [b, e]          = find_end(rng, to_find);
    REQUIRE(b == begin(rng) + 4);
    REQUIRE(e == end(rng));
#else
    auto it = find_end(rng, to_find);
    REQUIRE(it == begin(rng) + 4);
#endif
  }

  SECTION("not found") {
    const int to_find[] = {3, 2};
#ifdef USE_RANGE_V3
    auto [b, e]          = find_end(rng, to_find);
    REQUIRE(b == end(rng));
    REQUIRE(e == end(rng));
#else
    auto it = find_end(rng, to_find);
    REQUIRE(it == end(rng));
#endif
  }
}

TEST_CASE("find_first_of") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("found") {
    const int to_find[] = {4, 2};
    auto &&res          = find_first_of(rng, to_find);
    REQUIRE(res == begin(rng) + 1);
  }

  SECTION("not found") {
    const int to_find[] = {5};
    auto &&res          = find_first_of(rng, to_find);
    REQUIRE(res == end(rng));
  }
}

TEST_CASE("find_if") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("found") {
    auto &&res = find_if(rng, [](int i) { return i >= 2; });
    REQUIRE(res == begin(rng) + 1);
  }

  SECTION("not found") {
    auto &&res = find_if(rng, [](int i) { return i > 5; });
    REQUIRE(res == end(rng));
  }
}

TEST_CASE("find_if_not") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("found") {
    auto &&res = find_if_not(rng, [](int i) { return i < 2; });
    REQUIRE(res == begin(rng) + 1);
  }

  SECTION("not found") {
    auto &&res = find_if_not(rng, [](int i) { return i < 5; });
    REQUIRE(res == end(rng));
  }
}

TEST_CASE("for_each") {
  int sum  = 0;
  auto fun = [&](int i) { sum += i; };
  std::vector<int> v1{0, 2, 4, 6};
  auto &&[i, f] = for_each(v1, fun);
  REQUIRE(i == v1.end());
  REQUIRE((f(1), sum) == 13);
}

TEST_CASE("generate") {
  std::vector<int> out(5u);
  generate(out, [i = 0]() mutable { return 42 + i++; });
  check_equal(out, {42, 43, 44, 45, 46});
}

TEST_CASE("generate_n") {
  std::vector<int> out;
  generate_n(back_inserter(out), 5, [i = 0]() mutable { return 42 + i++; });
  check_equal(out, {42, 43, 44, 45, 46});
}

TEST_CASE("heap_algorithm") {
  std::vector<double> rng{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  make_heap(rng);
  check_equal(rng, {9, 8, 6, 7, 4, 5, 2, 0, 3, 1});

  rng.push_back(8.5);
  push_heap(rng);
  check_equal(rng,
             std::initializer_list<double>{9, 8.5, 6, 7, 8, 5, 2, 0, 3, 1, 4});

  pop_heap(rng);
  check_equal(rng,
             std::initializer_list<double>{8.5, 8, 6, 7, 4, 5, 2, 0, 3, 1, 9});

  REQUIRE_FALSE(is_heap(rng));

  auto i = is_heap_until(rng);
  REQUIRE(i == rng.begin() + 10);

  rng.pop_back();
  REQUIRE(is_heap(rng));
}

TEST_CASE("inplace_merge") {
  int rng[] = {0, 1, 2, 6, 7, 3, 4, 7, 8, 9};
  inplace_merge(rng, begin(rng) + 5);
  check_equal(rng, {0, 1, 2, 3, 4, 6, 7, 7, 8, 9});
}

TEST_CASE("is_paritioned") {
  int rng[] = {0, 2, 4, 8, 1, 3, 5, 7};

  SECTION("holds") {
    auto &&res = is_partitioned(rng, [](int i) { return i % 2 == 0; });
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res = is_partitioned(rng, [](int i) { return i % 3 == 0; });
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("is_sorted") {
  int rng[] = {0, 1, 2, 3, 4, 5, 6, 7};

  SECTION("holds") {
    auto &&res = is_sorted(rng);
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res = is_sorted(rng, std::greater{});
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("is_sorted_until") {
  int rng[]  = {0, 1, 2, 3, 4, 5, 3, 7};
  auto &&res = is_sorted_until(rng);
  REQUIRE(res == begin(rng) + 6);
}

TEST_CASE("lexicographical_compare") {
  SECTION("holds") {
    auto &&res =
      lexicographical_compare(views::c_str("less"), views::c_str("than"));
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res =
      lexicographical_compare(views::c_str("less"), views::c_str("less"));
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("lower_bound") {
  const int rng[] = {1, 2, 3, 3, 5};

  SECTION("found") {
    auto &&res = lower_bound(rng, 2);
    REQUIRE(res == begin(rng) + 1);
  }

  SECTION("not found") {
    auto &&res = lower_bound(rng, 4);
    REQUIRE(res == begin(rng) + 4);
  }
}

TEST_CASE("max") {
  const int rng[] = {4, 2, 3, 1, 4};

  auto &&res = max(rng);
  REQUIRE(res == 4);
}

TEST_CASE("max_element") {
#ifdef USE_RANGE_V3
  const int rng[] = {4, 2, 3, 1, 4};
#else
  https://github.com/CaseyCarter/cmcstl2/issues/306
  const int rng[] = {4, 2, 3, 1};
#endif

  auto &&res = max_element(rng);
  REQUIRE(res == begin(rng));
}

TEST_CASE("merge") {
  int rng0[] = {0, 1, 2, 6, 7};
  int rng1[] = {3, 4, 7, 8, 9};
  std::vector<int> out;
  merge(rng0, rng1, back_inserter(out));
  check_equal(out, {0, 1, 2, 3, 4, 6, 7, 7, 8, 9});
}

TEST_CASE("min") {
  const int rng[] = {4, 2, 3, 1, 4};

  auto &&res = min(rng);
  REQUIRE(res == 1);
}

TEST_CASE("min_element") {
  const int rng[] = {4, 2, 3, 1, 4};

  auto &&res = min_element(rng);
  REQUIRE(res == begin(rng) + 3);
}

TEST_CASE("minmax") {
  const int rng[] = {4, 2, 3, 1, 4};

  auto &&[min, max] = minmax(rng);
  REQUIRE(min == 1);
  REQUIRE(max == 4);
}

TEST_CASE("minmax_element") {
  const int rng[] = {4, 2, 3, 1, 4};

  auto &&[min, max] = minmax_element(rng);
  REQUIRE(min == begin(rng) + 3);
  REQUIRE(max == begin(rng) + 4);
}

TEST_CASE("mismatch") {
  SECTION("equals") {
    int rng0[]          = {1, 2, 3, 3, 4};
    int rng1[]          = {1, 2, 3, 3, 4};
    auto &&[pos0, pos1] = mismatch(rng0, rng1);
    REQUIRE(pos0 == end(rng0));
    REQUIRE(pos1 == end(rng1));
  }

  SECTION("not equals") {
    int rng0[]          = {1, 2, 3, 3, 4};
    int rng1[]          = {1, 2, 3, 3, 5};
    auto &&[pos0, pos1] = mismatch(rng0, rng1);
    REQUIRE(pos0 == begin(rng0) + 4);
    REQUIRE(pos1 == begin(rng1) + 4);
  }
}

TEST_CASE("move") {
  auto rng = views::iota(0, 5)
             | views::transform([](int i) { return std::vector<int>{i}; })
             | to_vector;
  std::vector<std::vector<int>> out;
  move(rng, back_inserter(out));
  check_equal(rng, std::initializer_list<std::vector<int>>{{}, {}, {}, {}, {}});
  check_equal(out, views::iota(0, 5) | views::transform([](int i) {
                    return std::vector<int>{i};
                  }));
}

TEST_CASE("move_backward") {
  auto rng = views::iota(0, 5)
             | views::transform([](int i) { return std::vector<int>{i}; })
             | to_vector;
  move_backward(begin(rng), begin(rng) + 3, end(rng));
  check_equal(rng, {std::vector<int>{}, std::vector<int>{}, std::vector<int>{0},
                   std::vector<int>{1}, std::vector<int>{2}});
}

TEST_CASE("none_of") {
  const int rng[] = {1, 2, 3, 3, 4};

  SECTION("holds") {
    auto &&res = none_of(rng, [](int i) { return i > 4; });
    REQUIRE(res);
  }

  SECTION("does not hold") {
    auto &&res = none_of(rng, [](int i) { return i == 4; });
    REQUIRE_FALSE(res);
  }
}

TEST_CASE("nth_element") {
  int rng[] = {4, 2, 3, 1, 4};

  nth_element(rng, begin(rng) + 3);

  REQUIRE(rng[3] == 4);
  REQUIRE(max(subrange(begin(rng), begin(rng) + 3)) < 4);
  REQUIRE(min(subrange(begin(rng) + 3, end(rng))) >= 4);
}

TEST_CASE("partial_sort") {
  int rng[] = {4, 2, 3, 1, 4};

  SECTION("in place") {
    partial_sort(rng, begin(rng) + 3);

    auto &&res = subrange(begin(rng), begin(rng) + 3);
    check_equal(res, {1, 2, 3});
  }

  SECTION("copy") {
    int out[3];

    partial_sort_copy(rng, out);

    check_equal(out, {1, 2, 3});
  }
}

TEST_CASE("partition") {
  int rng[]   = {7, 0, 2, 9, 3, 5, 4, 8, 6, 1};
  int evens[] = {0, 2, 4, 6, 8};
  int odds[]  = {1, 3, 5, 7, 9};

  SECTION("in place") {
    partition(rng, [](int i) { return i % 2 == 0; });

    REQUIRE(is_permutation(subrange(begin(rng), begin(rng) + 5), evens));
    REQUIRE(is_permutation(subrange(begin(rng) + 5, end(rng)), odds));
  }

  SECTION("copy") {
    int out0[5], out1[5];

    partition_copy(rng, out0, out1, [](int i) { return i % 2 == 0; });

    REQUIRE(is_permutation(out0, evens));
    REQUIRE(is_permutation(out1, odds));
  }
}

TEST_CASE("permutation") {
  int rng[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  SECTION("next_permutation") {
    next_permutation(rng);
    check_equal(rng, {0, 1, 2, 3, 4, 5, 6, 7, 9, 8});
  }

  SECTION("prev_permutation") {
    prev_permutation(rng);
    check_equal(rng, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
  }

  SECTION("is_permutation") {
    SECTION("holds") {
      int permuted[] = {7, 0, 2, 9, 3, 5, 4, 8, 6, 1};
      auto res       = is_permutation(rng, permuted);
      REQUIRE(res);
    }

    SECTION("does not hold") {
      int not_permuted[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 8};
      auto res           = is_permutation(rng, not_permuted);
      REQUIRE_FALSE(res);
    }
  }
}

TEST_CASE("remove") {
  int rng[] = {4, 2, 3, 1, 4};

  SECTION("in place") {
    SECTION("value") {
      auto &&it  = ranges::remove(rng, 4);
      auto &&res = subrange(begin(rng), it);
      check_equal(res, {2, 3, 1});
    }

    SECTION("predicate") {
      auto &&it  = remove_if(rng, [](int i) { return i % 2 == 0; });
      auto &&res = subrange(begin(rng), it);
      check_equal(res, {3, 1});
    }
  }

  SECTION("copy") {
    std::vector<int> out;

    SECTION("value") {
      remove_copy(rng, back_inserter(out), 4);
      check_equal(out, {2, 3, 1});
    }

    SECTION("predicate") {
      remove_copy_if(rng, back_inserter(out), [](int i) { return i % 2 == 0; });
      check_equal(out, {3, 1});
    }
  }
}

TEST_CASE("replace") {
  int rng[] = {4, 2, 3, 1, 4};

  SECTION("in place") {
    SECTION("value") {
      replace(rng, 4, 5);
      check_equal(rng, {5, 2, 3, 1, 5});
    }

    SECTION("predicate") {
      replace_if(rng, [](int i) { return i % 2 == 0; }, 5);
      check_equal(rng, {5, 5, 3, 1, 5});
    }
  }

  SECTION("copy") {
    std::vector<int> out;

    SECTION("value") {
      replace_copy(rng, back_inserter(out), 4, 5);
      check_equal(out, {5, 2, 3, 1, 5});
    }

    SECTION("predicate") {
      replace_copy_if(rng, back_inserter(out), [](int i) { return i % 2 == 0; },
                      5);
      check_equal(out, {5, 5, 3, 1, 5});
    }
  }
}

TEST_CASE("reverse") {
  int rng[] = {4, 2, 3, 1, 4};

  SECTION("in place") {
    reverse(rng);
    check_equal(rng, {4, 1, 3, 2, 4});
  }

  SECTION("copy") {
    int out[size(rng)];
    reverse_copy(rng, out);
    check_equal(out, {4, 1, 3, 2, 4});
  }
}

TEST_CASE("rotate") {
  int rng[] = {4, 2, 3, 1, 4};

  SECTION("in place") {
    rotate(rng, begin(rng) + 2);
    check_equal(rng, {3, 1, 4, 4, 2});
  }

  SECTION("copy") {
    int out[size(rng)];
    rotate_copy(rng, begin(rng) + 2, out);
    check_equal(out, {3, 1, 4, 4, 2});
  }
}

TEST_CASE("search") {
  auto rng   = views::iota(0);
  int to_search[] = {42, 43, 44};
  auto &&res  = search(rng, to_search);
  check_equal(res, to_search);
}

TEST_CASE("search_n") {
  int rng[] = {41, 42, 42, 42, 42, 43, 44};
  auto &&res  = search_n(rng, 4, 42);
#ifdef USE_RANGE_V3
  check_equal(res, {42, 42, 42, 42});
#else
  REQUIRE(res == begin(rng) + 1);
#endif
}

TEST_CASE("set_algorithms") {
  auto multiples_of_3 = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27};
  auto squares =
    views::iota(0) | views::transform([](int x) { return x * x; }) | views::take(6);
  std::vector<int> out;

  SECTION("set_difference") {
    set_difference(multiples_of_3, squares, back_inserter(out));
    check_equal(out, {3, 6, 12, 15, 18, 21, 24, 27});
  }

  SECTION("set_intersection") {
    set_intersection(multiples_of_3, squares, back_inserter(out));
    check_equal(out, {0, 9});
  }

  SECTION("set_union") {
    set_union(multiples_of_3, squares, back_inserter(out));
    check_equal(out, {0, 1, 3, 4, 6, 9, 12, 15, 16, 18, 21, 24, 25, 27});
  }

  SECTION("set_symmetric_difference") {
    set_symmetric_difference(multiples_of_3, squares, back_inserter(out));
    check_equal(out, {1, 3, 4, 6, 12, 15, 16, 18, 21, 24, 25, 27});
  }

  SECTION("includes") {
    SECTION("holds") {
      int rng0[] = {0, 1, 2, 3, 4, 5};
      int rng1[] = {2, 3, 4};
      auto &&res = includes(rng0, rng1);
      REQUIRE(res);
    }

    SECTION("does not hold") {
      int rng0[] = {0, 1, 2, 4, 5};
      int rng1[] = {2, 3, 4};
      auto &&res = includes(rng0, rng1);
      REQUIRE_FALSE(res);
    }
  }
}

TEST_CASE("shuffle") {
  int rng[] = {0, 1, 2, 3, 4, 5};
  shuffle(rng);
  REQUIRE(!is_sorted(rng));
  int sorted[] = {0, 1, 2, 3, 4, 5};
  REQUIRE(is_permutation(rng, sorted));
}

TEST_CASE("sort") {
  int rng[] = {2, 3, 1, 1, 5, 4};
  sort(rng);
  check_equal(rng, {1, 1, 2, 3, 4, 5});
}

TEST_CASE("stable_partition") {
  std::pair<int, int> rng[] = {{0, 7}, {1, 0}, {2, 2}, {3, 9}, {4, 3},
                               {5, 5}, {6, 4}, {7, 8}, {8, 6}, {9, 1}};

  stable_partition(rng, [](int i) { return i % 2 == 0; },
                   &std::pair<int, int>::second);
  check_equal(rng, std::initializer_list<std::pair<int, int>>{{1, 0},
                                                             {2, 2},
                                                             {6, 4},
                                                             {7, 8},
                                                             {8, 6},
                                                             {0, 7},
                                                             {3, 9},
                                                             {4, 3},
                                                             {5, 5},
                                                             {9, 1}});
}

TEST_CASE("stable_sort") {
  std::pair<int, int> rng[] = {{0, 2}, {1, 0}, {2, 2}, {3, 0}, {4, 3},
                               {5, 1}, {6, 1}, {7, 2}, {8, 0}, {9, 1}};
  stable_sort(rng, {}, &std::pair<int, int>::second);
  check_equal(rng, std::initializer_list<std::pair<int, int>>{{1, 0},
                                                             {3, 0},
                                                             {8, 0},
                                                             {5, 1},
                                                             {6, 1},
                                                             {9, 1},
                                                             {0, 2},
                                                             {2, 2},
                                                             {7, 2},
                                                             {4, 3}});
}

TEST_CASE("swap_ranges") {
  int rng0[] = {0, 1, 2, 3, 4, 5, 6};
  int rng1[] = {46, 45, 44, 43, 42, 41, 40};

  swap_ranges(rng0, rng1);

  check_equal(rng0, {46, 45, 44, 43, 42, 41, 40});
  check_equal(rng1, {0, 1, 2, 3, 4, 5, 6});
}

TEST_CASE("transform") {
  int rng[] = {0, 1, 2, 3, 4, 5, 6};
  int out[size(rng)];

  transform(rng, out, [](int i) { return i * 2; });

  check_equal(out, {0, 2, 4, 6, 8, 10, 12});
}

TEST_CASE("unique") {
  int rng[] = {0, 0, 1, 1, 1, 5, 5, 3, 4, 4, 4};

  SECTION("in place") {
    auto &&it  = unique(rng);
    auto &&res = subrange(begin(rng), it);
    check_equal(res, {0, 1, 5, 3, 4});
  }

  SECTION("copy") {
    std::vector<int> out;
    unique_copy(rng, back_inserter(out));

    check_equal(out, {0, 1, 5, 3, 4});
  }
}

TEST_CASE("upper_bound") {
  const int rng[] = {1, 2, 3, 3, 5};

  SECTION("found") {
    auto &&res = upper_bound(rng, 2);
    REQUIRE(res == begin(rng) + 2);
  }

  SECTION("not found") {
    auto &&res = upper_bound(rng, 4);
    REQUIRE(res == begin(rng) + 4);
  }
}
