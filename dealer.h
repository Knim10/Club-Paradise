#pragma once
#ifndef DEALER_H
#define DEALER_H

#include "person.h"
#include "deck.h"
#include <string>

/**
 * Dealer
 * - Inherits Person (hand, ace logic, clearHand, showHand, etc.)
 * - Follows house rules: hit on 16, stand on 17 (configurable soft-17 behavior)
 * - No bankroll/betting — the Table handles settlements against Players
 *
 * Helpers:
 *  - playHand(Deck&): play out the dealer's hand per rules
 *  - upCardValue(): value of the first (up) card; -1 if none
 *  - showUpCard(): print "[<upcard>, ?]"
 *  - isBlackjack(): true if exactly 2 cards totaling 21
 *  - isSoft(): true if current total uses Ace as 11 (i.e., a "soft" total)
 *  - setHitSoft17(bool): allow switching rules (default: stand on all 17)
 */
class Dealer : public Person {
public:
    Dealer() = default;
    ~Dealer() override = default;

    // Core play logic (hit on 16, stand on 17; optionally hit soft 17)
    void playHand(Deck& deck);

    // Rule configuration (default false = stand on soft 17)
    void setHitSoft17(bool enable) { hitSoft17 = enable; }
    bool getHitSoft17() const { return hitSoft17; }

    // Info helpers
    int  upCardValue() const;   // -1 if no cards in hand
    void showUpCard() const;    // prints "Dealer shows: [<up>, ?]"
    bool isBlackjack() const;   // exactly 2 cards, total 21
    bool isSoft() const;        // true if an Ace is counted as 11 in current total

private:
    // If true, dealer will hit soft 17 (A+6). Default is false (stand on any 17).
    bool hitSoft17 = false;
};

#endif // DEALER_H
