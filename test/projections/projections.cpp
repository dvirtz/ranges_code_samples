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
  std::string first_name;
  std::string last_name;
};

bool operator==(const employee &lhs, const employee &rhs) {
  return std::tie(lhs.first_name, lhs.last_name) ==
           std::tie(rhs.first_name, rhs.last_name);
}

TEST_CASE("employee") {
  std::vector<employee> employees{{"Eric", "Niebler"},
                                  {"Casey", "Carter"},
                                  {"Christopher", "Di Bella"},
                                  {"Corentin", "Jabot"}};

  SECTION("legacy") {
    std::sort(
      employees.begin(), employees.end(),
      [](const employee &x, const employee &y) { return x.last_name < y.last_name; });
    REQUIRE(employees
            == std::vector<employee>{{"Casey", "Carter"},
                                     {"Christopher", "Di Bella"},
                                     {"Corentin", "Jabot"},
                                     {"Eric", "Niebler"}});

    auto p = std::lower_bound(
      employees.begin(), employees.end(), "Parent",
      [](const employee &x, const std::string &y) { return x.last_name < y; });
    REQUIRE(p == employees.end());
  }

  SECTION("lambda") {
    using namespace ranges;
    auto get_last_name = [](const employee &e) { return e.last_name; };

    sort(employees, less{}, get_last_name);
    REQUIRE(employees
            == std::vector<employee>{{"Casey", "Carter"},
                                     {"Christopher", "Di Bella"},
                                     {"Corentin", "Jabot"},
                                     {"Eric", "Niebler"}});

    auto p = lower_bound(employees, "Parent", less{}, get_last_name);
    REQUIRE(p == employees.end());
  }

  SECTION("member") {
    using namespace ranges;
    sort(employees, {}, &employee::last_name);
    REQUIRE(employees
            == std::vector<employee>{{"Casey", "Carter"},
                                     {"Christopher", "Di Bella"},
                                     {"Corentin", "Jabot"},
                                     {"Eric", "Niebler"}});

    auto p = lower_bound(employees, "Parent", {}, &employee::last_name);
    REQUIRE(p == employees.end());
  }

  SECTION("find"){
    using namespace ranges;
    auto it = find(employees, "Sean", &employee::first_name);
    static_assert(std::is_same_v<decltype(*it), employee&>);

    auto v = employees | views::transform(&employee::first_name);
    auto it2 = find(v, "Sean");
    static_assert(std::is_same_v<decltype(*it2), std::string&>);
  }

  SECTION("lambda") {
    using namespace ranges;
    auto it = find(employees, 4, [](const employee& e){
      return e.first_name.size();
    });
    REQUIRE(it->first_name == "Eric");
  }
}