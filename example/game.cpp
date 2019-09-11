#include <codecvt>
#include <iostream>
#include <locale>
#include <map>
#include <random>
#include <range/v3/all.hpp>
#include <string>
#include <vector>
#include <iomanip>

using namespace ranges;

using Card = std::pair<std::string, std::string>;
using Deck = std::vector<Card>;

const auto SUITS = views::c_str(u8"♠ ♡ ♢ ♣") | views::split(u' ');
const auto RANKS =
  views::c_str(u8"2 3 4 5 6 7 8 9 10 J Q K A") | views::split(u' ');

/// Create a new deck of 52 cards
Deck createDeck(std::mt19937 &gen, bool shuffle = false) {
  auto deck = views::cartesian_product(SUITS, RANKS)
              | views::transform([](const auto &tuple) {
                  // https://github.com/ericniebler/range-v3/issues/1084
                  return std::pair{std::get<0>(tuple), std::get<1>(tuple)};
                }) 
              | to<Deck>;
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
auto CPP_fun(choose)(Rng &&rng, std::mt19937 &gen)(requires forward_range<Rng>) {
  auto sampled = views::iota(begin(rng), end(rng)) | views::sample(1, gen);
  return *sampled.begin();
}

/// Rotate player order so that start goes first
template <typename Rng>
auto CPP_fun(playerOrder)(Rng &&rng, iterator_t<Rng> startingPlayer)(requires range<Rng>) {
  return views::concat(subrange{startingPlayer, end(rng)},
                      subrange{begin(rng), startingPlayer});
}

/// Play a 4-player card game
void play() {
  auto gen = std::mt19937{std::random_device{}()};
  const auto deck  = createDeck(gen, true);
  const auto names = views::c_str(u8"P1 P2 P3 P4") | views::split(u' ') | to<std::vector<std::string>>;
  auto hands =
    views::zip(names, dealHands(deck)) | to<std::map<std::string, Deck>>;
  auto startingPlayer  = choose(names, gen);
  const auto turnOrder = playerOrder(names, startingPlayer);

  // Randomly play cards from each player's hand until empty
  while (!empty(hands[*startingPlayer])) {
    for (auto &&name : turnOrder) {
      const auto card = choose(hands[name], gen);
      std::cout << name << ": " << std::left << std::setw(7)
                << (views::concat(card->first, card->second) | to<std::string>);
      hands[name].erase(card);
    }
    std::cout << '\n';
  }
}

int main() { play(); }