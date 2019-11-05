#include "utility/missing_utilities.hpp"
#include <iomanip>
#include <iostream>
#include <random>
#include <range/v3/all.hpp>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if __cpp_char8_t >= 201811L
using char_t = char8_t;
#else
using char_t = char;
#endif
using string = std::basic_string<char_t>;

using Suit = string;
using Rank = string;

using namespace ranges;

static auto gen = std::mt19937{std::random_device{}()};

struct Card {
  Suit suit;
  Rank rank;

  friend std::basic_ostream<char_t> &
    operator<<(std::basic_ostream<char_t> &ost, const Card &card) {
    return ost << card.suit << std::left << std::setw(2) << card.rank;
  }
};

using Cards = std::vector<Card>;

struct Deck {
  Cards cards;

  Deck(bool shuffle = false) : cards(createDeck(shuffle)) {}

  /// Create a new deck of 52 cards
  static Cards createDeck(bool shuffle) {
    static const auto SUITS = views::c_str(u8"♠ ♥ ♦ ♣") | views::split(u8' ');
    static const auto RANKS =
      views::c_str(u8"2 3 4 5 6 7 8 9 10 J Q K A") | views::split(u8' '); // '
    auto cards = views::cartesian_product(SUITS, RANKS)
                 | views::transform([](const auto &tuple) {
                     auto &&[suit, rank] = tuple;
                     return Card{suit | to<Suit>(), rank | to<Rank>()};
                   })
                 | to_vector;
    if (shuffle) {
      return actions::shuffle(cards, gen);
    }

    return cards;
  }

  /// Deal the cards in the deck into a number of hands
  auto deal(ptrdiff_t numHands) const {
    auto slice = [this](ptrdiff_t from) {
      return cards | views::slice(from, end) | views::stride(4);
    };
    return views::indices(numHands) | views::transform(slice);
  }
};

/// Choose and return a random item
template <typename Rng>
auto choose(Rng &&rng)
  -> CPP_ret(iterator_t<Rng>)(requires ranges::forward_range<Rng>) {
  auto sampled = views::iota(begin(rng), end(rng)) | views::sample(1, gen);
  return *sampled.begin();
}

struct Player {
  string name;
  Cards hand;

  /// Play a card from the player's hand
  Card playCard() {
    auto it   = choose(hand);
    auto card = *it;
    hand.erase(it);
    std::basic_stringstream<char_t> sst;
    sst << name << ": " << card << "  ";
    const auto str = sst.str();
    std::fwrite(str.data(), 1, str.size(), stdout);
    return card;
  }
};

class Game {
private:
  using Players = std::vector<Player>;
  Players players;

  template <typename Names>
  static Players CPP_fun(createPlayers)(Names names)(
    requires(view_<Names> && same_as<range_value_t<Names>, const char_t*>)) {
    auto defaultNames = {u8"P1", u8"P2", u8"P3", u8"P4"};
    auto namesWithDefaults = views::concat(names, defaultNames)
                             | views::take_exactly(distance(defaultNames));
    return views::zip_with(
             [](auto &&name, auto &&cards) {
               return Player{name, to<Cards>(cards)};
             },
             namesWithDefaults, Deck{true}.deal(distance(namesWithDefaults)))
           | to<Players>();
  }

  /// Rotate player order so that start goes first
  auto playerOrder(Players::iterator startingPlayer) {
    return views::concat(subrange{startingPlayer, end(players)},
                         subrange{begin(players), startingPlayer});
  }

public:
  CPP_template(typename Names)(
    requires(view_<Names> && same_as<range_value_t<Names>, const char_t*>))
    Game(Names names) :
      players{createPlayers(names)} {}

  /// Play a card game
  void play() {
    auto startingPlayer = choose(players);
    auto turnOrder      = playerOrder(startingPlayer);
    // Randomly play cards from each player's hand until empty
    while (!empty(startingPlayer->hand)) {
      for_each(turnOrder, [](auto &player) { player.playCard(); });
      std::cout << '\n';
    }
  }
};

int main(int argc, const char_t *argv[]) {
  // Read player names from command line
  auto playerNames = views::counted(argv, argc) | views::drop_exactly(1);
  Game game{playerNames};
  game.play();
}