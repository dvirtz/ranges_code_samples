#include <algorithm>
#include <benchmark/benchmark.h>
#include <list>
#include <random>
#include <utility/missing_utilities.hpp>
#ifdef USE_RANGE_V3
#include <range/v3/view/counted.hpp>
#else
#include <experimental/ranges/ranges>
#endif

using namespace ranges;

template <typename I, typename S>
void CPP_fun(insertion_sort)(I begin, S end)(requires ranges::sentinel_for<S, I>) {
  for (auto it = begin; it != end; ++it) {
    auto insertion = std::upper_bound(begin, it, *it);
    std::rotate(insertion, it, next(it));
  }
}

auto createList(int64_t N) {
  std::mt19937 gen;
  std::uniform_int_distribution<> dis(0, static_cast<int>(N));
  std::list<int> list;
  std::generate_n(std::back_inserter(list), N, [&]() { return dis(gen); });
  return list;
}

static void naive(benchmark::State &state) {
  // Code inside this loop is measured repeatedly
  auto list = createList(state.range(0));
  for (auto _ : state) {
    insertion_sort(list.begin(), list.end());
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(list);
  }
}
// Register the function as a benchmark
BENCHMARK(naive)->Range(8, 8 << 11);

static void counted(benchmark::State &state) {
  // Code before the loop is not measured
  auto list = createList(state.range(0));
  for (auto _ : state) {
    auto counted = views::counted(list.begin(), list.size());
    insertion_sort(counted.begin(), counted.end());
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(list);
  }
}
BENCHMARK(counted)->Range(8, 8 << 11);

BENCHMARK_MAIN();
