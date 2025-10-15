/*
 * Blackjack - Game Driver
 * Author: Kenneth Nimmo
 * Date: 2025-10-02
 * Email: knimmo1@dmacc.edu
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
#include <iomanip>
#include "deck.h"
#include "table.h"
#include "player.h"
#include "dealer.h"

using namespace std;

// ---------- helpers ----------
// ---------- introduction & instructions ----------
static void showHowToPlay() {
    cout << "=========================================\n";
    cout << "           WELCOME TO BLACKJACK!\n";
    cout << "=========================================\n\n";
    cout << "How to Play:\n";
    cout << "-----------------------------------------\n";
    cout << "1. Each player starts with a set amount of money.\n";
    cout << "2. Before each round, players place a bet.\n";
    cout << "3. Each player and the dealer are dealt two cards.\n";
    cout << "   - Face cards (J, Q, K) count as 10.\n";
    cout << "   - Aces count as 11 or 1, whichever helps you most.\n";
    cout << "4. Each player takes turns choosing to 'Hit' (draw a card)\n";
    cout << "   or 'Stand' (keep current hand).\n";
    cout << "   If your hand exceeds 21, you 'bust' and lose your bet.\n";
    cout << "5. After all players finish, the dealer plays next:\n";
    cout << "   - Dealer hits on 16 and stands on 17 or higher.\n";
    cout << "6. Closest to 21 without going over wins even money!\n";
    cout << "   - If you tie the dealer, it's a push (no money lost).\n";
    cout << "-----------------------------------------\n";
    cout << "At any time, press 'H' when prompted to re-display these rules.\n";
    cout << "-----------------------------------------\n\n";
    cout << "Press Enter to start the game...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\n\n";
}

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
        cout << prompt << " (y/n or h for help): ";
        string s;
        getline(cin, s);
        if (s.empty()) continue;
        char c = tolower(static_cast<unsigned char>(s[0]));
        if (c == 'y') return true;
        if (c == 'n') return false;
        if (c == 'h') {
            showHowToPlay();
            continue; // re-ask after showing help
        }
        cout << "Please enter y, n, or h.\n";
    }
}

static int promptBetFor(Player& p, int maxBank) {
    while (true) {
        cout << "Enter bet for " << p.getName()
            << " ($1-" << maxBank << ", or h for help): ";
        string input;
        getline(cin, input);
        if (input.empty()) continue;
        if (tolower(input[0]) == 'h') {
            showHowToPlay();
            continue;
        }
        try {
            int bet = stoi(input);
            if (bet >= 1 && bet <= maxBank)
                return bet;
        }
        catch (...) {}
        cout << "Invalid bet. Enter a number between 1 and " << maxBank << ".\n";
    }
}

static void printHandInline(const Person& who) {
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

        cout << "Hit (H to view rules)? (y/n): ";
        string s;
        getline(cin, s);
        if (s.empty()) continue;
        char c = tolower(static_cast<unsigned char>(s[0]));
        if (c == 'y') {
            p.cardDealt(deck.deal());
            continue;
        }
        else if (c == 'n') {
            cout << p.getName() << " stands.\n";
            return;
        }
        else if (c == 'h') {
            showHowToPlay();
            continue;
        }
        else {
            cout << "Please enter y, n, or h.\n";
        }
    }
}

// Remove busted (bankrupt) players from table+list
static void pruneBrokePlayers(vector<unique_ptr<Player>>& roster, Table& table) {
    vector<Player*> survivors;
    survivors.reserve(roster.size());
    for (auto& up : roster) {
        survivors.push_back(up.get());
    }
    // Rebuild table players list:
    table.clearPlayers();
    for (auto* p : survivors) table.addPlayer(p);
}

// New helper: show round summary
static void printRoundSummary(const vector<unique_ptr<Player>>& roster, int roundNum) {
    cout << "\n=== Round " << roundNum << " Summary ===\n";
    cout << left << setw(15) << "Player" << setw(10) << "Bank ($)" << "\n";
    cout << string(25, '-') << "\n";
    for (const auto& up : roster) {
        cout << left << setw(15) << up->getName()
            << setw(10) << up->getMoney() << "\n";
    }
    cout << string(25, '-') << "\n";
}

static string signedMoney(int n) {
    // e.g., +125, -50, or 0
    if (n > 0) return "+" + to_string(n);
    return to_string(n);
}

static void printFinalReport(const vector<unique_ptr<Player>>& roster, int roundsPlayed) {
    cout << "\n=========================================\n";
    cout << "            FINAL GAME REPORT\n";
    cout << "=========================================\n";
    cout << "Rounds played: " << (roundsPlayed) << "\n\n";

    cout << left << setw(16) << "Player"
        << right << setw(12) << "Start($)"
        << right << setw(12) << "End($)"
        << right << setw(12) << "Net($)"
        << right << setw(12) << "Wins"
        << right << setw(8) << "Loss"
        << right << setw(8) << "Push"
        << "\n";

    cout << string(16 + 12 + 12 + 12 + 12 + 8 + 8, '-') << "\n";

    for (const auto& up : roster) {
        const int start = up->getStartingMoney();
        const int end = up->getMoney();
        const int net = up->getNet();

        cout << left << setw(16) << up->getName()
            << right << setw(12) << start
            << right << setw(12) << end
            << right << setw(12) << signedMoney(net)
            << right << setw(12) << up->getWins()
            << right << setw(8) << up->getLosses()
            << right << setw(8) << up->getPushes()
            << "\n";
    }

    cout << string(16 + 12 + 12 + 12 + 12 + 8 + 8, '-') << "\n\n";
}

// ---------- main ----------
int main() {
    cout << "=== Blackjack (Console) ===\n\n";

    showHowToPlay();

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

    for (auto& up : roster) table.addPlayer(up.get());

    bool keepPlaying = true;
    int roundNum = 1;

    while (keepPlaying) {
        cout << "\n--- New Round " << roundNum << " ---\n";

        // Get bets
        for (auto& up : roster) {
            int cap = up->getMoney();
            if (cap <= 0) {
                cout << up->getName() << " is out of money and will be skipped.\n";
                continue;
            }
            int bet = promptBetFor(*up, cap);
            up->setBet(bet);
        }

        // Deal, play, settle
        table.startRound();
        table.showDealerUpCard();
        for (auto& up : roster) {
            if (up->getMoney() <= 0) continue;
            playPlayerTurn(*up, deck);
        }

        table.dealerPlay();
        table.showDealerHand(true);
        table.settleBets();
        table.clearHands();

        // Show summary after settlement
        printRoundSummary(roster, roundNum);

        // Continue?
        keepPlaying = promptYesNo("Play another round?");
        if (!keepPlaying) {
            // Print final report before exiting
            printFinalReport(roster, /* roundsPlayed = */ roundNum);
            break;
        }
        ++roundNum;
    }

    cout << "\nThanks for playing!\n";
    return 0;
}

