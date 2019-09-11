#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>
#ifdef USE_RANGE_V3
#include <range/v3/all.hpp>
#else
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
namespace ranges = std::experimental::ranges;
#endif
#include <vector>

static constexpr auto square = [](int x) { return x * x; };

int naive_impl(int count) {
  std::vector<int> numbers(static_cast<size_t>(count));
  std::iota(numbers.begin(), numbers.end(), 1);
  std::transform(numbers.begin(), numbers.end(), numbers.begin(), square);
  return std::accumulate(numbers.begin(), numbers.end(), 0);
}

static void naive(benchmark::State &state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    int total = naive_impl(state.range(0));

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(total);
  }
}
// Register the function as a benchmark
BENCHMARK(naive)->Range(8 << 4, 8 << 16);

int for_loop_impl(int count) {
  int total = 0;
  for (auto i = 1; i <= count; ++i) {
    total += square(i);
  }
  return total;
}

static void for_loop(benchmark::State &state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    int total = for_loop_impl(state.range(0));

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(total);
  }
}
// Register the function as a benchmark
BENCHMARK(for_loop)->Range(8 << 4, 8 << 16);

int ranges_impl(int count) {
  using namespace ranges;

#ifdef USE_RANGE_V3
#ifdef FUNCTION_CALL
  return accumulate(
    views::take_exactly(
      views::transform(
        views::iota(1, unreachable), square
      )), 0);
#else
  return accumulate(
    views::iota(1) | views::transform(square) | views::take_exactly(count), 0);
#endif
#else
  auto squares = views::iota(1) | views::transform(square) | views::take(count) | views::common;
  return std::accumulate(squares.begin(), squares.end(), 0);
#endif
}

static void ranges_(benchmark::State &state) {
  // Code before the loop is not measured
  for (auto _ : state) {
    int total = ranges_impl(state.range(0));

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(total);
  }
}
BENCHMARK(ranges_)->Range(8 << 4, 8 << 16);

int ranges_common_impl(int count) {
  using namespace ranges;

  auto squares = views::iota(1) | views::transform(square) | views::take_exactly(count) | view::common;
  return std::accumulate(squares.begin(), squares.end(), 0);
}

static void ranges_common(benchmark::State &state) {
  // Code before the loop is not measured
  for (auto _ : state) {
    int total = ranges_impl(state.range(0));

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(total);
  }
}
BENCHMARK(ranges_common)->Range(8 << 4, 8 << 16);

BENCHMARK_MAIN();
