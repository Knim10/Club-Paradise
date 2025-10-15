/*
 * Table Class Implementation
 * ---------------------------
 * Manages the overall game flow for a round of Blackjack:
 *  - Holds references to the Dealer, Deck, and all Player objects.
 *  - Deals cards to players and dealer.
 *  - Controls dealer behavior and settles bets based on outcomes.
 *  - Provides helper methods for showing cards, clearing hands, and testing.
 *  - ChatGPT was used for comments only
 *
 * Author: Kenneth Nimmo
 * Date: 2025-10-02
 * Email: knimmo1@dmacc.edu
 */

#include "table.h"
#include "dealer.h"
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

/**
 * Table constructor
 * -----------------
 * Initializes the table with a reference to the shared Deck object.
 * The dealer is created automatically as part of the Table.
 */
Table::Table(Deck& inDeck) : deck(inDeck), players(), dealer() {}

/**
 * addPlayer(p)
 * ------------
 * Adds a Player pointer to the table’s player list.
 * The Table does not take ownership of the Player (no deletion).
 */
void Table::addPlayer(Player* p) {
    if (p) players.push_back(p);
}

/**
 * clearPlayers()
 * --------------
 * Removes all Player pointers from the table.
 * Does not delete Player objects, since they are managed externally.
 */
void Table::clearPlayers() {
    players.clear();
}

/**
 * playerCount()
 * --------------
 * Returns the number of players currently seated at the table.
 */
size_t Table::playerCount() {
    return players.size();
}

/**
 * dealOneToDealer()
 * -----------------
 * Deals one card to the dealer by drawing from the Deck.
 */
void Table::dealOneToDealer() {
    dealer.cardDealt(deck.deal());
}

/**
 * dealOneToPlayer(p)
 * ------------------
 * Deals one card to the specified Player by drawing from the Deck.
 */
void Table::dealOneToPlayer(Player& p) {
    p.cardDealt(deck.deal());
}

/**
 * startRound()
 * -------------
 * Begins a new round of Blackjack by:
 *  1. Clearing all player and dealer hands.
 *  2. Dealing two cards to each player and two to the dealer.
 */
void Table::startRound() {
    // Step 1: Clear all hands before dealing
    clearHands();

    // Step 2: Initial deal — first card to each player
    for (auto* p : players) {
        if (!p) continue;
        dealOneToPlayer(*p);
    }
    dealOneToDealer();  // Dealer’s first card

    // Step 3: Second card to each player
    for (auto* p : players) {
        if (!p) continue;
        dealOneToPlayer(*p);
    }
    dealOneToDealer();  // Dealer’s second card
}

/**
 * dealerPlay()
 * -------------
 * Calls the Dealer’s play logic (Dealer::playHand),
 * which continues until the dealer must stand.
 */
void Table::dealerPlay() {
    dealer.playHand(deck);
}

/**
 * settleBets()
 * -------------
 * Compares each player's hand against the dealer’s and adjusts
 * player balances based on outcomes:
 *  - Player bust -> lose bet
 *  - Dealer bust -> all remaining players win
 *  - Player > Dealer -> win bet
 *  - Player < Dealer -> lose bet
 *  - Tie -> push (no money changes hands)
 *
 * At the end of the round, the dealer’s hand is cleared.
 */
void Table::settleBets() {
    const int dVal = dealer.handValue();
    const bool dBust = dVal > 21;

    cout << "\n=== Settlements ===\n";
    cout << "Dealer: value=" << dVal << (dBust ? " (BUST)\n" : "\n");

    // Loop through each player and determine outcome
    for (auto* p : players) {
        if (!p) continue;

        const int bet = p->getBet();
        const int v = p->handValue();
        const bool bust = v > 21;

        cout << "Player [" << p->getName() << "] hand=" << v;

        // Case 1: Player busts immediately loses
        if (bust) {
            cout << " -> LOSS (-$" << bet << ")\n";
            p->handLost(bet);
            continue;
        }

        // Case 2: Dealer busts, player wins automatically
        if (dBust) {
            cout << " -> WIN (+$" << bet << ")\n";
            p->handWon(bet);
            continue;
        }

        // Case 3: Compare player vs dealer values
        if (v > dVal) {
            cout << " > dealer(" << dVal << ") -> WIN (+$" << bet << ")\n";
            p->handWon(bet);
        }
        else if (v < dVal) {
            cout << " < dealer(" << dVal << ") -> LOSS (-$" << bet << ")\n";
            p->handLost(bet);
        }
        else {
            // Case 4: Tie — push (no win/loss)
            cout << " = dealer(" << dVal << ") -> PUSH ($0)\n";
            // On push, hand is still cleared but no money moves.
            p->clearHand();
        }
    }

    // After all players settled, clear the dealer's hand for next round
    dealer.clearHand();
}

/**
 * showDealerUpCard()
 * -------------------
 * Displays only the dealer’s first card (the "up card").
 * Used at the beginning of each round before the dealer plays.
 */
void Table::showDealerUpCard() {
    dealer.showUpCard();
}

/**
 * showDealerHand(revealAll)
 * --------------------------
 * Displays the dealer’s full hand if revealAll == true.
 * Otherwise, hides the hole card (second card).
 */
void Table::showDealerHand(bool revealAll) {
    cout << "Dealer hand: ";
    if (revealAll) {
        dealer.showHand();
        cout << " value=" << dealer.handValue() << endl;
    }
    else {
        cout << "[?, ?]" << endl; // Keep hidden until showdown
    }
}

/**
 * showPlayers()
 * --------------
 * Displays each player's name, cards, hand value, and bet.
 * Useful for debugging or when showing the current table state.
 */
void Table::showPlayers() {
    for (auto* p : players) {
        if (!p) continue;
        cout << "Player [" << p->getName() << "] hand=";
        p->showHand();
        cout << " value=" << p->handValue() << " | bet=$" << p->getBet() << endl;
    }
}

/**
 * dealerHandValue()
 * ------------------
 * Returns the current total hand value of the dealer.
 */
int Table::dealerHandValue() {
    return dealer.handValue();
}

/**
 * dealerBusted()
 * ---------------
 * Returns true if the dealer’s hand exceeds 21.
 */
bool Table::dealerBusted() {
    return dealer.handValue() > 21;
}

/**
 * clearHands()
 * -------------
 * Clears all player and dealer hands at the start or end of a round.
 */
void Table::clearHands() {
    for (auto* p : players) {
        if (!p) continue;
        p->clearHand();
    }
    dealer.clearHand();
}

/**
 * testClearDealer()
 * ------------------
 * Testing helper that manually clears the dealer’s hand.
 * Used for unit testing and debugging.
 */
void Table::testClearDealer() {
    dealer.clearHand();
}

/**
 * testDealToDealer(card)
 * -----------------------
 * Testing helper that manually deals a specific card value to the dealer.
 * Used in deterministic test cases.
 */
void Table::testDealToDealer(int card) {
    dealer.cardDealt(card);
}
