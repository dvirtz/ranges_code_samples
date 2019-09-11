#include <benchmark/benchmark.h>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/single.hpp>
#include <fstream>
#include <iostream>

template <ranges::range Rng> auto count_lines_in_files(Rng &&files) {
  auto open_file = [](const std::string &filename) {
    return std::ifstream(filename);
  };

  auto count_lines = [](std::ifstream file) {
    return ranges::count(std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>(),
                         '\n');
  };

  return files | ranges::views::transform(open_file)
         | ranges::views::transform(count_lines);
}

static void rangify(benchmark::State &state) {
  for (auto _ : state) {
    auto lines =
      count_lines_in_files(ranges::views::single(std::string{__FILE__}));
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(lines);
  }
}
BENCHMARK(rangify)->Range(8, 8 << 11);

template <ranges::range Rng> auto count_lines_in_files_2(Rng &&files) {
  std::vector<int> results;
  char c = 0;

  for (const auto &file : files) {
    int line_count = 0;

    std::ifstream in(file);

    while (in.get(c)) {
      if (c == '\n') {
        line_count++;
      }
    }

    results.push_back(line_count);
  }

  return results;
}

static void naive(benchmark::State &state) {
  for (auto _ : state) {
    auto lines =
      count_lines_in_files_2(ranges::views::single(std::string{__FILE__}));
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(lines);
  }
}
BENCHMARK(naive)->Range(8, 8 << 11);

BENCHMARK_MAIN();