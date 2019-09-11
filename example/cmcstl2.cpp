#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
#include <cassert>

namespace ranges = std::experimental::ranges;

int main() {
  using namespace ranges;
  auto v = views::single(42);
  auto u = {42};
  auto [l, r] = ranges::mismatch(v, u);
  assert(l == v.end());
  assert(r = end(u));
}