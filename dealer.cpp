#include "dealer.h"
#include <iostream>

using std::cout;
using std::endl;

/*
* Kenneth Nimmo
* Knimmo1@dmacc.edu
* 10/2/2025
* 
*/
void Dealer::playHand(Deck& deck) {
    // Keep it simple for stub: just hit until 17+
    while (handValue() <= 16) {
        cardDealt(deck.deal());
    }
}

int Dealer::upCardValue() const {
    if (hand.empty()) return -1;
    return hand.front(); // 'hand' is protected in Person, so accessible here
}

void Dealer::showUpCard() const {
    cout << "Dealer shows: [";
    if (hand.empty()) {
        cout << "?, ?]\n";
        return;
    }
    cout << upCardValue() << ", ?]" << endl;
}
