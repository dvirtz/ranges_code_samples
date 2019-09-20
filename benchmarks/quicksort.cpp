#include <algorithm>
#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <utility/view.hpp>
#ifdef USE_RANGE_V3
#include <range/v3/all.hpp>
#else
#include <experimental/ranges/ranges>
#endif

auto createList(size_t N) {
  std::mt19937 gen;
  std::uniform_int_distribution<> dis(0, N);
  std::vector<int> list;
  std::generate_n(std::back_inserter(list), N, [&]() { return dis(gen); });
  return list;
}

static void naive(benchmark::State &state) {
  // Code inside this loop is measured repeatedly
  auto list = createList(state.range(0));
  for (auto _ : state) {
    ranges::sort(list);
    auto min = ranges::front(list);
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(list);
  }
}
// Register the function as a benchmark
BENCHMARK(naive)->Range(8, 8 << 11);

template<ranges::forward_range R>
auto quick_sort(R &&r) -> ranges::any_view<ranges::range_value_t<R>, ranges::category::forward>{
  using namespace ranges;
  if (empty(std::forward<R>(r))) {
    return std::forward<R>(r);
  }

  auto pivot = front(std::forward<R>(r));
  auto tail = r | views::tail;
  auto cond = [&pivot](auto x){
      return x <= pivot;
    };
  return views::concat(
    quick_sort(tail | views::filter(cond)),
    views::single(pivot),
    quick_sort(tail | views::remove_if(cond)));
}

static void lazy(benchmark::State &state) {
  // Code before the loop is not measured
  auto list = createList(state.range(0));
  for (auto _ : state) {
    auto min = ranges::front(quick_sort(list));
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(list);
  }
}
BENCHMARK(lazy)->Range(8, 8 << 11);

BENCHMARK_MAIN();
