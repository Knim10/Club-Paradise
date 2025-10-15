#include "player.h"
#include <iostream>
using namespace std;

Player::Player(string inName, int inMoney)
    : name(std::move(inName)), money(inMoney), startingMoney(inMoney), bet(0) {
}

void Player::setBet(int betAmount) {
    if (betAmount > money) {
        cout << "Bet can't be greater than your current bank amount: $" << money << endl;
        return;
    }
    bet = betAmount;
}

int Player::getBet() const {
    return bet;
}

void Player::handWon(int moneyWon) {
    money += moneyWon;
    ++wins;
    clearHand();
    ace = false;
}

void Player::handLost(int moneyLost) {
    money -= moneyLost;
    ++losses;
    clearHand();
    ace = false;
}

void Player::handPush() {
    ++pushes;
    clearHand();
    ace = false;
}

const string& Player::getName() const {
    return name;
}

int Player::getMoney() const {
    return money;
}
