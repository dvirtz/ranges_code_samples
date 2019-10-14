#include "utility/missing_utilities.hpp"
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <random>
#include <range/v3/all.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace game {
using namespace ranges;

#if __cpp_char8_t >= 201811L
using char_t = char8_t;
#else
using char_t = char;
#endif
using string = std::basic_string<char_t>;

template <typename View>
auto operator<<(std::basic_ostream<char_t> &ost, View view)
  -> CPP_ret(std::basic_ostream<char_t> &)(
    requires view_<View> &&same_as<range_value_t<View>, char_t>) {
  for_each(view, [&ost](char_t c) { ost << c; });
  return ost;
}

struct RangeComparator {
  using is_transparent = void;

  template <typename LHS, typename RHS>
  bool CPP_fun(operator())(LHS &&lhs, RHS &&rhs)(const requires(
    range<LHS> &&range<RHS>
      &&totally_ordered_with<range_value_t<LHS>, range_value_t<RHS>>)) {
    return lexicographical_compare(lhs, rhs);
  }
};

using Card = std::pair<string, string>;
using Deck = std::vector<Card>;

const auto SUITS = views::c_str(u8"♠ ♥ ♦ ♣") | views::split(u8' ');
const auto RANKS =
  views::c_str(u8"2 3 4 5 6 7 8 9 10 J Q K A") | views::split(u8' '); // '

/// Create a new deck of 52 cards
Deck createDeck(std::mt19937 &gen, bool shuffle = false) {
  auto deck = views::cartesian_product(SUITS, RANKS)
              | views::transform([](const auto &tuple) {
                  const auto &[suit, rank] = tuple;
                  // https://github.com/ericniebler/range-v3/issues/1084
                  return Card{suit | to<string>(), rank | to<string>()};
                })
              | to<Deck>();
  if (shuffle) {
    return deck | move | actions::shuffle(gen);
  }

  return deck;
}

/// Deal the cards in the deck into four hands
auto dealHands(const Deck &deck) {
  auto slice = [&deck](int from) {
    return deck | views::slice(from, end) | views::stride(4);
  };
  return views::ints(0, 4) | views::transform(slice);
}

/// Choose and return a random item
template <typename Rng>
auto CPP_fun(choose)(Rng &&rng,
                     std::mt19937 &gen)(requires forward_range<Rng>) {
  auto sampled = views::iota(begin(rng), end(rng)) | views::sample(1, gen);
  return *sampled.begin();
}

/// Rotate player order so that start goes first
template <typename Rng>
auto CPP_fun(playerOrder)(Rng &&rng,
                          iterator_t<Rng> startingPlayer)(requires range<Rng>) {
  return views::concat(subrange{startingPlayer, end(rng)},
                       subrange{begin(rng), startingPlayer});
}

/// Play a 4-player card game
void play() {
  auto gen         = std::mt19937{std::random_device{}()};
  const auto deck  = createDeck(gen, true);
  const auto names = views::split(views::c_str(u8"P1 P2 P3 P4"), u8' '); //'
  auto hands       = views::zip_with(
                 [](auto name, auto hand) {
                   return std::pair{name | to<string>(), hand | to<Deck>()};
                 },
                 names, dealHands(deck))
               | to<std::map<string, Deck, RangeComparator>>();
  auto startingPlayer  = choose(names, gen);
  const auto turnOrder = playerOrder(names, startingPlayer);

  // Randomly play cards from each player's hand until empty

  const auto &startingHand = hands.find(*startingPlayer)->second;
  while (!empty(startingHand)) {
    for (auto name : turnOrder) {
      auto &hand       = hands.find(name)->second;
      const auto &card = choose(hand, gen);
      std::basic_stringstream<char_t> sst;
      static_assert(view_<decltype(name)>);
      sst << name << u8": " << std::setw(7) << std::left
                                  << views::concat(card->first, card->second);
      const auto &str = sst.str();
      std::fwrite(str.data(), 1, str.size(), stdout);
      hand.erase(card);
    }
    std::cout << "\n";
  }
}

} // namespace game

int main() { game::play(); }
