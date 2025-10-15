/*
* Isaac coddington 9/30
*/
#include "person.h"
#include <iostream>

//adds card to hand and checks if card is an ace
void Person::cardDealt(int card) {
    if (card == 11) {
        ace = true;
    }
    hand.push_back(card);
};
//returns hand value
int Person::handValue() const {
    int value = 0;
    for (int i : hand) {
        value += i;
    };
    //if hand value is > 21 and ace == true this subtracts 10 from value
    if (value > 21 && ace == true) {
        value = value - 10;
    }
    return value;
};
//prints cards held as so [1, 2, 3, }
void Person::showHand() const {
    cout << "[";
    for (int i : hand) {
        cout << i << ", ";
    };
    cout << "]";
};
//clears hand vector of all cards
void Person::clearHand() {
    hand.clear();
    ace = false;
    return;
}
