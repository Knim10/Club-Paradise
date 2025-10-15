/*
 * Dealer Class Implementation
 * ----------------------------
 * Defines the behavior of the Dealer in the Blackjack game.
 *
 * Responsibilities:
 *  - Automatically plays its hand according to house rules:
 *      * Hits on totals of 16 or less.
 *      * Stands on hard 17+.
 *      * Optionally hits soft 17 if 'hitSoft17' flag is true.
 *  - Provides helper methods to reveal cards, determine blackjack,
 *    and evaluate whether the hand is "soft" (contains an Ace counted as 11).
 *  - ChatGPT was used for comments
 *
 * Author: Kenneth Nimmo
 * Date: 2025-10-02
 * Email: knimmo1@dmacc.edu
 */

#include "dealer.h"
#include <iostream>

using std::cout;
using std::endl;

/**
 * playHand(deck)
 * --------------
 * Controls the dealer's turn logic.
 * - Continues drawing cards until reaching a total of 17 or higher.
 * - If the hand is a "soft 17" (Ace counted as 11),
 *   behavior depends on 'hitSoft17' (true = hit, false = stand).
 *
 * The method exits automatically once the dealer must stand.
 */
void Dealer::playHand(Deck& deck) {
    while (true) {
        int total = handValue();

        // Stop if dealer has 17+ and it's a hard hand (no soft Ace)
        if (total >= 17 && !isSoft()) break;

        // If total == 17 and hand is soft (e.g., A+6),
        // decide based on the hitSoft17 rule.
        if (total == 17 && isSoft()) {
            if (hitSoft17) {
                cardDealt(deck.deal());  // Dealer hits soft 17
                continue;                // Check new total
            }
            else {
                break;                   // Stand on soft 17
            }
        }

        // Hit on totals of 16 or less
        if (total <= 16) {
            cardDealt(deck.deal());
            continue;
        }

        // Default case: stop drawing
        break;
    }
}

/**
 * upCardValue()
 * --------------
 * Returns the value of the dealer's first (face-up) card.
 * If no cards have been dealt yet, returns -1.
 */
int Dealer::upCardValue() const {
    if (hand.empty()) return -1;
    return hand.front();
}

/**
 * showUpCard()
 * -------------
 * Displays only the dealer's up card (first card), hiding the rest.
 * Used during gameplay before the dealer’s full hand is revealed.
 */
void Dealer::showUpCard() const {
    cout << "Dealer shows: [";
    if (hand.empty()) {
        cout << "?, ?]" << endl;  // Dealer has not yet been dealt cards
        return;
    }
    cout << upCardValue() << ", ?]" << endl;
}

/**
 * isBlackjack()
 * --------------
 * Returns true if the dealer has exactly two cards totaling 21.
 * Used to detect a natural blackjack (Ace + 10-value card).
 */
bool Dealer::isBlackjack() const {
    if (hand.size() != 2) return false;
    return handValue() == 21;
}

/**
 * isSoft()
 * ---------
 * Determines whether the dealer’s hand is "soft."
 * A hand is considered soft if it contains at least one Ace counted as 11
 * without the total exceeding 21.
 *
 * Example:
 *   A + 6 = soft 17 (true)
 *   A + 6 + 10 = hard 17 (false)
 *
 * Implementation notes:
 * - Counts aces as 11 initially, reducing them to 1 as needed to avoid busts.
 * - Returns true if at least one Ace remains valued at 11 after adjustments.
 */
bool Dealer::isSoft() const {
    int sum = 0;
    int aces = 0;

    // Step 1: Count all aces as 11 initially
    for (int v : hand) {
        if (v == 11) {
            ++aces;
            sum += 11;
        }
        else {
            sum += v;
        }
    }

    // Step 2: Reduce Aces (subtract 10) while sum > 21
    while (sum > 21 && aces > 0) {
        sum -= 10;
        --aces;
    }

    // Step 3: Recompute total to determine if any Ace remains as 11
    int recompute = 0;
    int aces2 = 0;
    for (int v : hand) {
        if (v == 11) {
            ++aces2;
            recompute += 11;
        }
        else {
            recompute += v;
        }
    }

    int reduced = 0;
    while (recompute > 21 && aces2 > 0) {
        recompute -= 10;
        --aces2;
        ++reduced;
    }

    // Step 4: If any ace still counted as 11 and total <= 21, it's soft.
    return (aces2 > 0 && recompute <= 21);
}
