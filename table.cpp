#include "table.h"
#include "dealer.h"
#include <iostream>
#include <iomanip>


/*
* Kenneth Nimmo
* Knimmo1@dmacc.edu
* 10/2/2025
*/
using std::cout;
using std::endl;

Table::Table(Deck& inDeck) : deck(inDeck), players(), dealer() {}

void Table::addPlayer(Player* p) {
    if (p) players.push_back(p);
}

void Table::clearPlayers() {
    players.clear();
}

size_t Table::playerCount(){
    return players.size();
}

void Table::dealOneToDealer() {
    dealer.cardDealt(deck.deal());
}

void Table::dealOneToPlayer(Player& p) {
    p.cardDealt(deck.deal());
}

void Table::startRound() {
    // Clear all hands first
    clearHands();

    // Initial deal: 2 to each player, 2 to dealer
    for (auto* p : players) {
        if (!p) continue;
        dealOneToPlayer(*p);
    }
    dealOneToDealer();

    for (auto* p : players) {
        if (!p) continue;
        dealOneToPlayer(*p);
    }
    dealOneToDealer();
}

void Table::dealerPlay() {
    // House rule: hit on 16, stand on 17
    while (dealer.handValue() <= 16) {
        dealOneToDealer();
    }
}

void Table::settleBets() {
    const int dVal = dealer.handValue();
    const bool dBust = dVal > 21;

    cout << "\n=== Settlements ===\n";
    cout << "Dealer: value=" << dVal << (dBust ? " (BUST)\n" : "\n");

    for (auto* p : players) {
        if (!p) continue;

        const int bet = p->getBet();
        const int v = p->handValue();
        const bool bust = v > 21;

        cout << "Player ";
        cout << "[" << p->getName() << "] ";
        cout << "hand=" << v;

        if (bust) {
            cout << " -> LOSS (-$" << bet << ")\n";
            p->handLost(bet);
            continue;
        }

        if (dBust) {
            cout << " -> WIN (+$" << bet << ")\n";
            p->handWon(bet);
            continue;
        }

        if (v > dVal) {
            cout << " > dealer(" << dVal << ") -> WIN (+$" << bet << ")\n";
            p->handWon(bet);
        }
        else if (v < dVal) {
            cout << " < dealer(" << dVal << ") -> LOSS (-$" << bet << ")\n";
            p->handLost(bet);
        }
        else {
            cout << " = dealer(" << dVal << ") -> PUSH ($0)\n";
            // On push, no money moves; but wethe player hand still needs to be cleared
            p->clearHand();
        }
    }

    // Dealer hand is done for the round; clear it now.
    dealer.clearHand();
}

void Table::showDealerUpCard(){
    cout << "Dealer shows: ";
    cout << "[?, ?]" << endl;
}

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

void Table::showPlayers() {
    for (auto* p : players) {
        if (!p) continue;
        cout << "Player [" << p->getName() << "] hand=";
        p->showHand();
        cout << " value=" << p->handValue() << " | bet=$" << p->getBet() << endl;
    }
}

int Table::dealerHandValue() {
    return dealer.handValue();
}

bool Table::dealerBusted() {
    return dealer.handValue() > 21;
}

void Table::clearHands() {
    // Clear all player hands (use Player/Person API)
    for (auto* p : players) {
        if (!p) continue;
        p->clearHand();
    }
    dealer.clearHand();
}

void Table::testClearDealer() {
    dealer.clearHand();
}

void Table::testDealToDealer(int card) {
    dealer.cardDealt(card);
}
