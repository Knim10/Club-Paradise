/*
* idcoddington@dmacc.edu isaac coddington
* 9/28/25
*
* Kenneth Nimmo Knimmo1@dmacc.edu
* 10/2/2025
*
* midterm group project
*/

#include "deck.h"
#include "player.h"
#include "dealer.h"
#include "person.h"
#include "table.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// ---------------- Simple test harness ----------------
struct TestStats {
    int passed = 0;
    int failed = 0;
    void check(bool cond, const string& name) {
        if (cond) { ++passed; cout << "[PASS] " << name << "\n"; }
        else { ++failed; cout << "[FAIL] " << name << "\n"; }
    }
} TS;

#define CHECK(expr) TS.check((expr), #expr)

static void section(const string& title) {
    cout << "\n==== " << title << " ====\n";
}

int main()
{
    // -------------------------------------------------
    // Person tests
    // -------------------------------------------------
    section("Person");
    {
        Person p;
        p.cardDealt(11); // Ace
        p.cardDealt(9);
        CHECK(p.handValue() == 20);   // 11+9 = 20

        p.cardDealt(5);               // 11+9+5 = 25 -> soft reduce to 15
        CHECK(p.handValue() == 15);

        p.clearHand();
        CHECK(p.handValue() == 0);
    }

    // -------------------------------------------------
    // Player tests
    // -------------------------------------------------
    section("Player");
    {
        Player alice("Alice", 500);
        CHECK(alice.getName() == "Alice");
        CHECK(alice.getMoney() == 500);
        CHECK(alice.getBet() == 0);

        alice.setBet(100);
        CHECK(alice.getBet() == 100);

        // win: +bet
        alice.handWon(alice.getBet());
        CHECK(alice.getMoney() == 600);
        CHECK(alice.getWins() == 1);

        // loss: -bet
        alice.setBet(50);
        alice.handLost(alice.getBet());
        CHECK(alice.getMoney() == 550);
        CHECK(alice.getLosses() == 1);

        // push: no change
        int before = alice.getMoney();
        alice.handPush();
        CHECK(alice.getMoney() == before);
        CHECK(alice.getPushes() == 1);

        // net vs starting (500)
        CHECK(alice.getNet() == (alice.getMoney() - alice.getStartingMoney()));
        CHECK(alice.getNet() == 50);
    }

    // -------------------------------------------------
    // Deck tests
    // -------------------------------------------------
    section("Deck");
    {
        Deck d;
        d.shuffle();

        auto v = d.viewDeck();
        CHECK(v.size() == 52);

        auto countVal = [&](int val) {
            return static_cast<int>(count(v.begin(), v.end(), val));
            };
        for (int k = 2; k <= 9; ++k) CHECK(countVal(k) == 4);
        CHECK(countVal(10) == 16);
        CHECK(countVal(11) == 4);

        // deal reduces size by 1
        int c1 = d.deal(); (void)c1;
        auto v2 = d.viewDeck();
        CHECK(v2.size() == 51);

        // drain remaining 51
        for (int i = 0; i < 51; ++i) d.deal();

        // viewDeck when empty should rebuild to 52
        auto v3 = d.viewDeck();
        CHECK(v3.size() == 52);

        // empty again then deal() should reshuffle+pop -> 51 left
        for (int i = 0; i < 52; ++i) d.deal();
        d.deal();
        auto v4 = d.viewDeck();
        CHECK(v4.size() == 51);
    }

    // -------------------------------------------------
    // Dealer helper tests (no randomness)
    // -------------------------------------------------
    section("Dealer helpers");
    {
        Dealer dl;
        CHECK(dl.upCardValue() == -1);

        // blackjack (two-card 21)
        dl.cardDealt(10);
        dl.cardDealt(11);
        CHECK(dl.isBlackjack() == true);

        // soft 17
        dl.clearHand();
        dl.cardDealt(11);
        dl.cardDealt(6);
        CHECK(dl.isSoft() == true);
        CHECK(dl.handValue() == 17);

        // hard 17
        dl.clearHand();
        dl.cardDealt(10);
        dl.cardDealt(7);
        CHECK(dl.isSoft() == false);
        CHECK(dl.handValue() == 17);
    }

    // -------------------------------------------------
    // Your original TABLE test cases (kept, with checks)
    // -------------------------------------------------
    section("Table (original scenarios)");
    Deck deck;
    deck.shuffle();

    Player alice("Alice", 1000);
    Player bob("Bob", 1000);

    Table table(deck);
    table.addPlayer(&alice);
    table.addPlayer(&bob);

    // -------------------------------
    // Test Case 1: Player beats dealer
    // Alice: 19, Bob: 16, Dealer: 17 (stands) -> Alice win, Bob loss
    // -------------------------------
    cout << "\n=== Table Test Case 1: Player beats dealer ===\n";
    alice.setBet(100);
    bob.setBet(100);

    // clear everyone
    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(9);   // 19
    bob.cardDealt(8);    bob.cardDealt(8);     // 16

    // dealer's hand (hard-code to 17)
    table.testClearDealer();
    table.testDealToDealer(10);
    table.testDealToDealer(7);                 // 17

    // no need to call dealerPlay(); dealer is already set
    table.settleBets();

    // checks
    CHECK(alice.getMoney() == 1100);
    CHECK(bob.getMoney() == 900);
    CHECK(alice.getWins() >= 1); // cumulative count across scenarios

    // -------------------------------
    // Test Case 2: Dealer busts
    // Alice: 17, Bob: 16, Dealer: 24 (bust) -> both win
    // -------------------------------
    cout << "\n=== Table Test Case 2: Dealer busts ===\n";
    alice.setBet(50);
    bob.setBet(50);

    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(7);   // 17
    bob.cardDealt(9);    bob.cardDealt(7);     // 16

    // dealer's hand (hard-code to a bust total)
    table.testClearDealer();
    table.testDealToDealer(10);
    table.testDealToDealer(6);
    table.testDealToDealer(8);                 // 24 -> bust

    table.settleBets();

    // checks (win both)
    CHECK(alice.getMoney() == 1150); // +50 from 1100
    CHECK(bob.getMoney() == 950); // +50 from 900
    CHECK(alice.getWins() >= 2);
    CHECK(bob.getWins() >= 1);

    // -------------------------------
    // Test Case 3: Push
    // Alice: 17, Bob: 17, Dealer: 17 -> both push
    // -------------------------------
    cout << "\n=== Table Test Case 3: Push ===\n";
    alice.setBet(25);
    bob.setBet(25);

    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(7);   // 17
    bob.cardDealt(9);    bob.cardDealt(8);     // 17

    // dealer's hand (hard-code to 17)
    table.testClearDealer();
    table.testDealToDealer(9);
    table.testDealToDealer(8);                 // 17

    table.settleBets();

    // Money should NOT change on push
    CHECK(alice.getMoney() == 1150);
    CHECK(bob.getMoney() == 950);

    cout << "\n=== Table Tests complete ===\n";

    // -------------------------------------------------
    // Summary
    // -------------------------------------------------
    cout << "\n================================\n";
    cout << "Tests passed: " << TS.passed << "\n";
    cout << "Tests failed: " << TS.failed << "\n";
    cout << "================================\n";

    return (TS.failed == 0) ? 0 : 1;
}
