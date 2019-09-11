#include <iomanip>
#include <iostream>
#include <random>
#include <range/v3/all.hpp>
#include <set>
#include <string>
#include <vector>

using Suit = std::string;
using Rank = std::string;

using namespace ranges;

static auto gen = std::mt19937{std::random_device{}()};

struct Card {
  Suit suit;
  Rank rank;

  friend std::ostream &operator<<(std::ostream &ost, const Card &card) {
    return ost << card.suit << std::left << std::setw(2) << card.rank;
  }
};

using Cards = std::vector<Card>;

struct Deck {
  Cards cards;

  Deck(bool shuffle = false) : cards(createDeck(shuffle)) {}

  /// Create a new deck of 52 cards
  static Cards createDeck(bool shuffle) {
    static const auto SUITS = views::c_str(u8"♠ ♡ ♢ ♣") | views::split(u8' ');
    static const auto RANKS =
      views::c_str(u8"2 3 4 5 6 7 8 9 10 J Q K A") | views::split(u8' ');
    auto cards =
      views::cartesian_product(SUITS, RANKS)
      | views::transform([](const auto &tuple) {
          return Card{std::get<0>(tuple) | to<Suit>, std::get<1>(tuple) | to<Rank>};
        })
      | to_vector;
    if (shuffle) {
      return actions::shuffle(cards, gen);
    }

    return cards;
  }

  /// Deal the cards in the deck into a number of hands
  auto deal(int numHands) const {
    auto slice = [this](int from) {
      return cards | views::slice(from, end) | views::stride(4);
    };
    return views::indices(numHands) | views::transform(slice);
  }
};

/// Choose and return a random item
template <typename Rng>
auto choose(Rng &&rng) -> CPP_ret(iterator_t<Rng>)(requires ranges::forward_range<Rng>) {
  auto sampled = views::iota(begin(rng), end(rng)) | views::sample(1, gen);
  return *sampled.begin();
}

struct Player {
  std::string name;
  Cards hand;

  /// Play a card from the player's hand
  Card playCard() {
    auto it   = choose(hand);
    auto card = *it;
    hand.erase(it);
    std::cout << name << ": " << card << "  ";
    return card;
  }
};

struct Game {
  using Players = std::vector<Player>;
  Players players;

  template <typename Names>
  Game(Names &&names) : players{createPlayers(std::forward<Names>(names))} {}

  static Players createPlayers(any_view<const char *> names) {
    auto defaultNames      = {"P1", "P2", "P3", "P4"};
    auto namesWithDefaults = views::concat(names, defaultNames)
                             | views::take_exactly(distance(defaultNames));
    return views::zip_with(
      [](auto &&name, auto &&cards) {
        return Player{name, cards | to<Cards>};
      },
      namesWithDefaults, Deck{true}.deal(distance(namesWithDefaults)))
      | to<Players>;
  }

  /// Rotate player order so that start goes first
  auto playerOrder(Players::iterator startingPlayer) {
    return views::concat(subrange{startingPlayer, end(players)},
                        subrange{begin(players), startingPlayer});
  }

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

int main(int argc, const char *argv[]) {
  // Read player names from command line
  auto playerNames = views::counted(argv, argc) | views::drop_exactly(1);
  Game game{playerNames};
  game.play();
}