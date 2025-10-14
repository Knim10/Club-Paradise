#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include "deck.h"
#include "player.h"
#include "person.h"

using namespace std;

/**
* Kenneth Nimmo
* Knimmo1@dmacc.edu
* 10/2/2025
* 
 * Table
 * - Owns the round flow for players vs. dealer.
 * - Holds player pointers (players live elsewhere).
 * - Uses Deck to deal.
 * - Dealer follows house rules: hit on 16, stand on 17.
 *
 * Payouts:
 *  - Even money (+bet on win, -bet on loss).
 *  - Push returns nothing (no money moved).
 *  - No blackjack/insurance/surrender here to keep it aligned with current APIs.
 */
class Table {
public:
    explicit Table(Deck& inDeck);

    // player management
    void addPlayer(Player* p);
    void clearPlayers();
    size_t playerCount();

    // round flow helpers
    void startRound();  // clears all hands, deals 2 to everyone (players + dealer)
    void dealerPlay();  // dealer hits on 16, stands on 17+
    void settleBets();  // pays wins, collects losses, handles pushes

    // display helpers
    void showDealerUpCard();  // shows dealer's first card only
    void showDealerHand(bool revealAll = false);
    void showPlayers();

    // query helpers
    int dealerHandValue();
    bool dealerBusted();

    // cleanup (e.g., after settleBets if you want to force-clear)
    void clearHands();

    // --- TEST HOOKS (use only in tests) ---
    void testClearDealer();  // clears dealer hand
    void testDealToDealer(int card); // deals a specific card to dealer (no deck)

private:
    Deck& deck;
    vector<Player*> players;
    Person dealer;  // Simple dealer; no bankroll tracked

    // internal helpers
    void dealOneToDealer();
    void dealOneToPlayer(Player& p);
};

#endif // TABLE_H
