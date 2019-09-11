#include <catch2/catch.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/numeric.hpp>
#include "test/range_matcher.hpp"

using namespace ranges;

TEST_CASE("for_each_n") {
  int sum  = 0;
  auto fun = [&](int i) { sum += i; };
  std::vector<int> v1{0, 2, 4, 6};
  auto i = for_each_n(begin(v1), 3, fun);
  REQUIRE(i == v1.begin() + 3);
  REQUIRE(sum == 6);
}

TEST_CASE("sample") {
  const int rng[] = {0, 1, 2, 3, 4, 5};
  int out[4];
  sample(rng, out);
  REQUIRE(is_sorted(out));
  REQUIRE(includes(rng, out));
}

TEST_CASE("accumulate") {
  auto rng = views::indices(10);
  auto &&res = accumulate(rng, 1);
  REQUIRE(res == 46);
}

TEST_CASE("adjacent_difference") {
  int rng[] = {5, 4, 2, 1, 5, 2};
  int out[size(rng)];
  adjacent_difference(rng, out);
  check_equal(out, {5, -1, -2, -1, 4, -3});
}

TEST_CASE("iota") {
  int rng[10];
  iota(rng, 42);
  check_equal(rng, views::ints(42, 52));
}

TEST_CASE("inner_product") {
  int rng0[] = {1, 2, 3, 4, 5, 6};
  int rng1[] = {42, 43, 44, 45, 46, 47};
  auto &&res = inner_product(rng0, rng1, 1);
  REQUIRE(res == 953);
}

TEST_CASE("partial_sum") {
  int rng[] = {5, 4, 2, 1, 5, 2};
  int out[size(rng)];
  partial_sum(rng, out);
  check_equal(out, {5, 9, 11, 12, 17, 19});
}
