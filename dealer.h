#pragma once
#ifndef DEALER_H
#define DEALER_H

#include "person.h"
#include "deck.h"

/**
*
* Kenneth Nimmo
* Knimmo1@dmacc.edu
* 10/2/2025
* 
 * Dealer (stub)
 * - Inherits Person: uses cardDealt(), handValue(), showHand(), clearHand()
 * - Provides playHand(Deck&) to follow house rules: hit on 16, stand on 17+
 * - Provides upCardValue() and showUpCard() convenience methods
 *
 * No bankroll or betting logic here.
 */
class Dealer : public Person {
public:
    Dealer() = default;
    ~Dealer() override = default;

    // House rules: hit on 16 or less, stand on 17 or more
    void playHand(Deck& deck);

    // Returns first (up) card value, or -1 if no cards
    int upCardValue() const;

    // Prints something like "Dealer shows: [10, ?]"
    void showUpCard() const;
};

#endif // DEALER_H
