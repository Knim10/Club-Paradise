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
 * - ChatGPT was used for comments and some debugging assistance only
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

// ====================================================
// Introduction & User Instructions
// ====================================================

/**
 * showHowToPlay()
 * Prints a one-page “How to Play” guide and waits for the user
 * to press Enter. Players can also request this at prompts by
 * entering 'h'.
 */
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

// ====================================================
// Small Input/Output Helpers
// ====================================================

/**
 * clearCin()
 * Clears error flags and flushes the rest of the current line
 * from std::cin so the next read is clean.
 */
static void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/**
 * promptInt(prompt, minVal, maxVal)
 * Repeatedly prompts until the user enters an integer in range.
 * Returns the validated integer.
 */
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

/**
 * promptString(prompt, minLen)
 * Repeatedly prompts until a non-empty (length >= minLen) line is entered.
 * Returns the entered string.
 */
static string promptString(const string& prompt, size_t minLen = 1) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (s.size() >= minLen) return s;
        cout << "Please enter at least " << minLen << " character(s).\n";
    }
}

/**
 * promptYesNo(prompt)
 * Asks a yes/no question. Accepts 'y', 'n', or 'h' to view the rules.
 * Returns true for 'y', false for 'n'. Loops on invalid input.
 */
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
            showHowToPlay();          // show rules on demand
            continue;                 // then re-ask the question
        }
        cout << "Please enter y, n, or h.\n";
    }
}

/**
 * promptBetFor(player, maxBank)
 * Prompts for a wager from 1..maxBank. If user types 'h', shows
 * rules and re-prompts. Returns the validated bet.
 */
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
        catch (...) {
            // fall through to error message
        }
        cout << "Invalid bet. Enter a number between 1 and " << maxBank << ".\n";
    }
}

/**
 * printHandInline(who)
 * Prints a Person’s hand in one line with the current value.
 * (Relies on Person::showHand() formatting.)
 */
static void printHandInline(const Person& who) {
    who.showHand();
    cout << " (" << who.handValue() << ")";
}

// ====================================================
// Core Gameplay Helpers
// ====================================================

/**
 * playPlayerTurn(player, deck)
 * Runs one player's turn: repeatedly show their hand, then ask
 * "Hit?" If they hit, deal a card. If they stand, stop. If they
 * bust (hand > 21), announce and end turn.
 *
 * Larger method sections:
 *  - Show current hand/value
 *  - Bust check exits early
 *  - Prompt for action (y/n/h), apply decision
 */
static void playPlayerTurn(Player& p, Deck& deck) {
    while (true) {
        // Show the current state of this player's hand
        cout << p.getName() << "'s hand: ";
        p.showHand();
        cout << " value=" << p.handValue() << "\n";

        // If already busted, end the turn immediately
        if (p.handValue() > 21) {
            cout << p.getName() << " busts!\n";
            return;
        }

        // Prompt for the next action: Hit, Stand, or Help
        cout << "Hit (H to view rules)? (y/n): ";
        string s;
        getline(cin, s);
        if (s.empty()) continue;
        char c = tolower(static_cast<unsigned char>(s[0]));
        if (c == 'y') {
            // Deal one card and continue loop
            p.cardDealt(deck.deal());
            continue;
        }
        else if (c == 'n') {
            // End the player's turn
            cout << p.getName() << " stands.\n";
            return;
        }
        else if (c == 'h') {
            // Show rules and re-ask
            showHowToPlay();
            continue;
        }
        else {
            cout << "Please enter y, n, or h.\n";
        }
    }
}

/**
 * pruneBrokePlayers(roster, table)
 * (Currently a no-op in terms of removal)
 * Rebuilds the Table’s player list from the current roster.
 * If you later decide to remove broke players, this is the place
 * to filter them out before re-adding to the Table.
 */
static void pruneBrokePlayers(vector<shared_ptr<Player>>& roster, Table& table, vector<shared_ptr<Player>>& outPlayers) {
    for (auto it = roster.begin(); it != roster.end(); ) {
        if ((*it)->getMoney() <= 0){
            outPlayers.push_back(*it);
            it = roster.erase(it);
        } else {
            it++;
        }
    }
    vector<Player*> survivors;
    survivors.reserve(roster.size());
    for (auto& up : roster) {
        survivors.push_back(up.get());
    }
    // Rebuild the table’s internal vector from survivors
    table.clearPlayers();
    for (auto* p : survivors) table.addPlayer(p);
}

/**
 * printRoundSummary(roster, roundNum)
 * Prints a small two-column summary of player names and current
 * bankrolls after each round.
 */
static void printRoundSummary(const vector<shared_ptr<Player>>& roster, int roundNum) {
    cout << "\n=== Round " << roundNum << " Summary ===\n";
    cout << left << setw(15) << "Player" << setw(10) << "Bank ($)" << "\n";
    cout << string(25, '-') << "\n";
    for (const auto& up : roster) {
        cout << left << setw(15) << up->getName()
            << setw(10) << up->getMoney() << "\n";
    }
    cout << string(25, '-') << "\n";
}

/**
 * signedMoney(n)
 * Formats an integer money delta with a + sign if positive.
 * e.g., +125, -50, 0
 */
static string signedMoney(int n) {
    if (n > 0) return "+" + to_string(n);
    return to_string(n);
}

/**
 * printFinalReport(roster, roundsPlayed)
 * Prints a final table when the user quits showing, for each
 * player, starting funds, ending funds, net delta, and
 * win/loss/push counts collected by Player.
 */
static void printFinalReport(const vector<shared_ptr<Player>>& roster, int roundsPlayed) {
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

// ====================================================
// Main Program
// ====================================================

/**
 * main()
 * High-level loop of the game:
 *  1) Show rules
 *  2) Create deck/table and collect players
 *  3) For each round:
 *     - Collect bets
 *     - Deal two cards to everyone
 *     - Let each player play their turn
 *     - Dealer plays, then Table settles bets
 *     - Print a round summary
 *     - Ask to continue; on 'no', print final report and exit
 */
int main() {
    cout << "=== Blackjack (Console) ===\n\n";

    // One-time instructions screen (press Enter to continue)
    showHowToPlay();

    // Core game objects
    Deck deck;
    deck.shuffle();
    Table table(deck);

    // --- Player setup ---
    int nPlayers = promptInt("How many players (1-4)? ", 1, 4);
    vector<shared_ptr<Player>> roster;
    vector<shared_ptr<Player>> outPlayers;
    roster.reserve(nPlayers);

    for (int i = 0; i < nPlayers; ++i) {
        string name = promptString("Enter player " + to_string(i + 1) + " name: ");
        int bank = promptInt("Starting bank for " + name + " ($100-$10000): ", 100, 10000);
        roster.emplace_back(make_unique<Player>(name, bank));
    }

    // Register players with Table
    for (auto& up : roster) table.addPlayer(up.get());

    bool keepPlaying = true;
    int roundNum = 1;

    // ===========================
    // Round Loop
    // ===========================
    while (keepPlaying) {
        cout << "\n--- New Round " << roundNum << " ---\n";

        // --- Betting phase ---
        for (auto& up : roster) {
            int cap = up->getMoney();
            if (cap <= 0) {
                // Player is broke; keep them in roster but skip the round
                cout << up->getName() << " is out of money and will be skipped.\n";
                up->setBet(0);
                continue;
            }
            int bet = promptBetFor(*up, cap);
            up->setBet(bet);
        }

        // --- Initial deal (2 to each player and dealer) ---
        table.startRound();

        // Show the dealer's up-card (depends on your Table/Dealer implementation)
        table.showDealerUpCard();

        // --- Each player's turn ---
        for (auto& up : roster) {
            if (up->getMoney() <= 0) continue;   // skip broke players
            playPlayerTurn(*up, deck);
        }

        // --- Dealer plays, then settle bets vs. dealer ---
        table.dealerPlay();
        table.showDealerHand(true);              // reveal dealer hand
        table.settleBets();
        table.clearHands();                      // just to be safe; some clears already happen

        // --- Round summary and continuation prompt ---
        printRoundSummary(roster, roundNum);
        pruneBrokePlayers(roster, table, outPlayers);
        if (roster.empty()) {
            keepPlaying = false;
        }else{
        keepPlaying = promptYesNo("Play another round?");
        }
        if (!keepPlaying) {
            for (auto& i : outPlayers){
                roster.push_back(i);
            }
            // On exit, show a final report with net results and W/L/P
            printFinalReport(roster, /* roundsPlayed = */ roundNum);
            break;
        }
        ++roundNum;
    }

    cout << "\nThanks for playing!\n";
    return 0;
}
