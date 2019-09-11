#include <algorithm>
#include <catch2/catch.hpp>
#ifdef USE_RANGE_V3
#include <range/v3/algorithm.hpp>
#include <range/v3/view/transform.hpp>
#else
#include <experimental/ranges/algorithm>
#include <experimental/ranges/ranges>
namespace ranges = std::experimental::ranges;
#endif
#include <string>

struct employee {
  std::string first;
  std::string last;
};

bool operator==(const employee &lhs, const employee &rhs) {
  return std::tie(lhs.first, lhs.last) ==
           std::tie(rhs.first, rhs.last);
}

TEST_CASE("employee") {
  std::vector<employee> employees{{"Eric", "Niebler"},
                                  {"Casey", "Carter"},
                                  {"Christopher", "Di Bella"},
                                  {"Corentin", "Jabot"}};

  SECTION("legacy") {
    std::sort(
      employees.begin(), employees.end(),
      [](const employee &x, const employee &y) { return x.last < y.last; });
    REQUIRE(employees
            == std::vector<employee>{{"Casey", "Carter"},
                                     {"Christopher", "Di Bella"},
                                     {"Corentin", "Jabot"},
                                     {"Eric", "Niebler"}});

    auto p = std::lower_bound(
      employees.begin(), employees.end(), "Parent",
      [](const employee &x, const std::string &y) { return x.last < y; });
    REQUIRE(p == employees.end());
  }

  SECTION("ranges") {
    using namespace ranges;
    sort(employees, {}, &employee::last);
    REQUIRE(employees
            == std::vector<employee>{{"Casey", "Carter"},
                                     {"Christopher", "Di Bella"},
                                     {"Corentin", "Jabot"},
                                     {"Eric", "Niebler"}});

    auto p = lower_bound(employees, "Parent", {}, &employee::last);
    REQUIRE(p == employees.end());
  }

  SECTION("find"){
    using namespace ranges;
    auto it = find(employees, "Sean", &employee::first);
    static_assert(std::is_same_v<decltype(*it), employee&>);

    auto v = employees | views::transform(&employee::first);
    auto it2 = find(v, "Sean");
    static_assert(std::is_same_v<decltype(*it2), std::string&>);
  }

  SECTION("lambda") {
    using namespace ranges;
    auto it = find(employees, 4, [](const employee& e){
      return e.first.size();
    });
    REQUIRE(it->first == "Eric");
  }
}