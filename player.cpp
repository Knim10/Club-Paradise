#include "player.h"
#include <iostream>

Player::Player(string inName, int inMoney) : name(inName), money(inMoney), bet() {}

//this will set bet variable to how much the player wants to bet on the hand if betAmount < money
void Player::setBet(int betAmount) {
    if (betAmount > money) {
        cout << "Bet can't be greater then your current bank amount: $" << money << endl;
        return;
    }
    else {
        bet = betAmount;
        return;
    }
};
//returns current bet amount
int Player::getBet() {
    return bet;
};
//will add moneyWon to money the player has
void Player::handWon(int moneyWon) {
    money += moneyWon;
    clearHand();
    ace = false;
    return;
};
//will subtract moneyLost from money the player has
void Player::handLost(int moneyLost) {
    money -= moneyLost;
    clearHand();
    ace = false;
    return;
};
//returns player name as a string
const string& Player::getName() {
    return name;
}

