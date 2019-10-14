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

template <typename F> static auto do_benchmark(benchmark::State &state, F &&f) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    const auto total = std::forward<F>(f)(static_cast<int>(state.range(0)));

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(total);
  }
}

#define DO_BENCHMARK(func)                                                     \
  BENCHMARK_CAPTURE(do_benchmark, func, func)->Range(8 << 4, 8 << 16);

int classic_stl(int count) {
  std::vector<int> numbers(static_cast<size_t>(count));
  std::iota(numbers.begin(), numbers.end(), 1);
  std::transform(numbers.begin(), numbers.end(), numbers.begin(),
                 [](int x) { return x * x; });
  return std::accumulate(numbers.begin(), numbers.end(), 0);
}

DO_BENCHMARK(classic_stl)

int for_loop(int count) {
  int total = 0;
  for (auto i = 1; i <= count; ++i) {
    total += i * i;
  }
  return total;
}

DO_BENCHMARK(for_loop)

int ranges_function_call(int count) {
  using namespace ranges;
  return accumulate(
    views::transform(
      views::iota(1, count), 
      [](int x) { return x * x; }
    ), 0
  );
}

DO_BENCHMARK(ranges_function_call)

int ranges_pipeline(int count) {
  using namespace ranges;
  auto squares = views::iota(1, count) 
               | views::transform([](int x) { return x * x; });
  return accumulate(squares, 0);
}

DO_BENCHMARK(ranges_pipeline)

BENCHMARK_MAIN();
