#include <catch2/catch.hpp>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/memory>
#include <experimental/ranges/ranges>
#include "test/range_matcher.hpp"

using namespace ranges;

struct S {
  S() { ++instances; }
  S(int i) : i{i} { ++instances; }
  S(const S &s) : i{s.i} { ++instances; }
  S(S &&s) : i{s.i} {
    s.i = 0;
    ++instances;
  }
  ~S() noexcept { --instances; }

  S &operator=(const S &) = default;
  S &operator=(S &&) = default;

  int i = 0;
  static size_t instances;
};
size_t S::instances = 0;

TEST_CASE("uninitalized_memory") {
  constexpr size_t N = 10;
  auto data          = std::allocator<S>{}.allocate(N);
  auto buffer        = views::counted(data, N);

  SECTION("regular") {
    SECTION("uninitialized_default_construct") {
      uninitialized_default_construct(buffer);
    }

    SECTION("uninitialized_value_construct") {
      uninitialized_value_construct(buffer);
    }

    SECTION("uninitialized_fill") {
      uninitialized_fill(buffer, S{42});
      check_equal(buffer | views::transform(&S::i), std::vector<int>(N, 42));
    }

    SECTION("uninitialized_copy") {
      auto ints = views::iota(0, static_cast<int>(N));
      uninitialized_copy(ints, buffer);
      check_equal(buffer | views::transform(&S::i), ints);
    }

    SECTION("uninitialized_move") {
      auto ints = views::iota(0, static_cast<int>(N));
      auto source =
        ints | views::transform([](int i) { return S{i}; }) | to_vector;
      uninitialized_move(source, buffer);
      REQUIRE(S::instances == 2 * N);
      check_equal(buffer | views::transform(&S::i), ints);
      check_equal(source | views::transform(&S::i), std::vector<int>(N, 0));
    }

    REQUIRE(S::instances == N);
    destroy(buffer);
  }

  SECTION("counted") {
    constexpr size_t K = 5;

    SECTION("uninitalized_default_construct_n") {
      uninitialized_default_construct_n(begin(buffer), K);
    }

    SECTION("uninitialized_value_construct_n") {
      uninitialized_value_construct_n(begin(buffer), K);
    }

    SECTION("uninitialized_fill_n") {
      uninitialized_fill_n(begin(buffer), K, S{42});
      check_equal(buffer | views::transform(&S::i) | views::take(K),
                 std::vector<int>(K, 42));
    }

    SECTION("uninitialized_copy_n") {
      auto ints = views::iota(0, static_cast<int>(K));
      uninitialized_copy_n(begin(ints), K, begin(buffer), end(buffer));
      check_equal(buffer | views::transform(&S::i) | views::take(K), ints);
    }

    SECTION("uninitialized_move_n") {
      auto ints = views::iota(0, static_cast<int>(K));
      auto source =
        ints | views::transform([](int i) { return S{i}; }) | to_vector;
      uninitialized_move_n(begin(source), K, begin(buffer), end(buffer));
      REQUIRE(S::instances == 2 * K);
      check_equal(buffer | views::transform(&S::i) | views::take(K), ints);
      check_equal(source | views::transform(&S::i), std::vector<int>(K, 0));
    }

    REQUIRE(S::instances == K);
    destroy_n(begin(buffer), K);
  }

  REQUIRE(S::instances == 0);
  std::allocator<S>{}.deallocate(data, N);
}