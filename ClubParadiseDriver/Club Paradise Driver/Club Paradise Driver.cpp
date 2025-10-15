/*
 * Blackjack - Game Driver (separate from tests)
 * Author: Your Name
 * Date: 2025-10-02
 *
 * Notes:
 * - Uses even-money settlements (no blackjack 3:2, no splits/doubles/insurance).
 * - Table.startRound() deals 2 to each player and 2 to dealer.
 * - Players decide hit/stand; then dealer plays (hit 16, stand 17).
 * - Deck auto-reshuffles when empty, per your Deck::deal() implementation.
 */

#include <iostream>
#include <limits>
#include <vector>
#include <memory>
#include "../Club paradise/table.h"
#include "../Club paradise/deck.h"
#include "../Club paradise/player.h"
#include "../Club paradise/person.h"
#include "../Club paradise/dealer.h"
 // only needed if your Table now uses Dealer internally

using namespace std;

// ---------- helpers ----------
static void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int promptInt(const string& prompt, int minVal, int maxVal) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= minVal && v <= maxVal) {
            clearCin();
            return v;
        }
        cout << "Please enter an integer between " << minVal << " and " << maxVal << ".\n";
        clearCin();
    }
}

static string promptString(const string& prompt, size_t minLen = 1) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s.size() >= minLen) return s;
        cout << "Please enter at least " << minLen << " character(s).\n";
    }
}

static bool promptYesNo(const string& prompt) {
    while (true) {
        cout << prompt << " (y/n): ";
        string s; getline(cin, s);
        if (s.size() == 0) continue;
        char c = tolower(static_cast<unsigned char>(s[0]));
        if (c == 'y') return true;
        if (c == 'n') return false;
        cout << "Please enter y or n.\n";
    }
}

static int promptBetFor(Player& p, int maxBank) {
    while (true) {
        int bet = promptInt("Enter bet for " + p.getName() + " ($1-" + to_string(maxBank) + "): ", 1, maxBank);
        // Player::setBet already guards, but we'll pre-check to be friendly
        if (bet <= maxBank) return bet;
        cout << "Bet cannot exceed current bank $" << maxBank << ". Try again.\n";
    }
}

static void printHandInline(const Person& who) {
    // Person::showHand() prints with trailing comma per your code; reuse it:
    who.showHand();
    cout << " (" << who.handValue() << ")";
}

// Let a player take actions (hit/stand) until they stand or bust
static void playPlayerTurn(Player& p, Deck& deck) {
    while (true) {
        cout << p.getName() << "'s hand: ";
        p.showHand();
        cout << " value=" << p.handValue() << "\n";

        if (p.handValue() > 21) {
            cout << p.getName() << " busts!\n";
            return;
        }

        bool hit = promptYesNo("Hit?");
        if (!hit) {
            cout << p.getName() << " stands.\n";
            return;
        }
        p.cardDealt(deck.deal());
    }
}

// Remove busted (bankrupt) players from table+list
static void pruneBrokePlayers(vector<unique_ptr<Player>>& roster, Table& table) {
    vector<Player*> survivors;
    survivors.reserve(roster.size());
    for (auto& up : roster) {
        // Money is private; we can infer bank via bet settle. If you want a true getter, add getMoney().
        // For now we won't remove; or you can track separately. To keep it simple, keep all players.
        survivors.push_back(up.get());
    }
    // Rebuild table players list:
    table.clearPlayers();
    for (auto* p : survivors) table.addPlayer(p);
}

// ---------- main game ----------
int main() {
    cout << "=== Blackjack (Console) ===\n\n";

    // Setup deck + table
    Deck deck;
    deck.shuffle();
    Table table(deck);

    // Setup players
    int nPlayers = promptInt("How many players (1-4)? ", 1, 4);
    vector<unique_ptr<Player>> roster;
    roster.reserve(nPlayers);

    for (int i = 0; i < nPlayers; ++i) {
        string name = promptString("Enter player " + to_string(i + 1) + " name: ");
        int bank = promptInt("Starting bank for " + name + " ($100-$10000): ", 100, 10000);
        roster.emplace_back(make_unique<Player>(name, bank));
    }

    // Add to table
    for (auto& up : roster) {
        table.addPlayer(up.get());
    }

    bool keepPlaying = true;
    while (keepPlaying) {
        cout << "\n--- New Round ---\n";

        // Get bets
        for (auto& up : roster) {
            // If you add Player::getMoney() later, use it here. For now, assume a sensible cap like $1000 for demo.
            // Better approach: track money in Player and expose getMoney(). Your Player already adjusts money in handWon/handLost.
            int assumedCap = 100000; // effectively unlimited; Player::setBet will reject if > money.
            int bet = promptBetFor(*up, assumedCap);
            up->setBet(bet);
        }

        // Initial deal
        table.startRound();

        // Show dealer up-card (your current Table::showDealerUpCard prints "[?, ?]"; if you switch to Dealer.showUpCard(), it will show actual upcard)
        table.showDealerUpCard();

        // Each player's turn
        for (auto& up : roster) {
            playPlayerTurn(*up, deck);
        }

        // Dealer plays + settle
        table.dealerPlay();
        table.showDealerHand(true); // reveal
        table.settleBets();

        // Clear hands done inside settleBets() for dealer and players on push/win/loss;
        // but we’ll force-clear any leftover (safe):
        table.clearHands();

        // Continue?
        keepPlaying = promptYesNo("\nPlay another round?");
        // Optionally prune broke players (needs getMoney() to be meaningful)
        pruneBrokePlayers(roster, table);
        if (roster.empty()) {
            cout << "All players are out. Game over.\n";
            break;
        }
    }

    cout << "\nThanks for playing!\n";
    return 0;
}
