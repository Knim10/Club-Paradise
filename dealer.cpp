#include "dealer.h"
#include <iostream>

using std::cout;
using std::endl;

void Dealer::playHand(Deck& deck) {
    while (true) {
        int total = handValue();
        if (total >= 17 && !isSoft()) break;

        if (total == 17 && isSoft()) {
            if (hitSoft17) {
                cardDealt(deck.deal());
                continue;
            }
            else {
                break;
            }
        }

        if (total <= 16) {
            cardDealt(deck.deal());
            continue;
        }
        break;
    }
}

int Dealer::upCardValue() const {
    if (hand.empty()) return -1;
    return hand.front();
}

void Dealer::showUpCard() const {
    cout << "Dealer shows: [";
    if (hand.empty()) {
        cout << "?, ?]" << endl;
        return;
    }
    cout << upCardValue() << ", ?]" << endl;
}

bool Dealer::isBlackjack() const {
    if (hand.size() != 2) return false;
    return handValue() == 21;
}

bool Dealer::isSoft() const {
    int sum = 0;
    int aces = 0;
    for (int v : hand) {
        if (v == 11) {
            ++aces;
            sum += 11;
        }
        else {
            sum += v;
        }
    }
    // Reduce as needed
    while (sum > 21 && aces > 0) {
        sum -= 10;
        --aces;
    }
    int recompute = 0;
    int aces2 = 0;
    for (int v : hand) {
        if (v == 11) { ++aces2; recompute += 11; }
        else { recompute += v; }
    }
    int reduced = 0;
    while (recompute > 21 && aces2 > 0) {
        recompute -= 10;
        --aces2;
        ++reduced;
    }
    // If we have any aces left at 11 (aces2 > 0) and total <= 21, it's soft.
    return (aces2 > 0 && recompute <= 21);
}
