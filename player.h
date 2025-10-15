#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "person.h"
#include <string>
using namespace std;

class Player : public Person
{
private:
    string name;
    int money;          // current bankroll
    int startingMoney;  // snapshot of starting bankroll for final report
    int bet;

    // Round stats
    int wins = 0;
    int losses = 0;
    int pushes = 0;

public:
    explicit Player(string inName, int inMoney);

    // Identity / money
    const string& getName() const;   // made const-safe
    int getMoney() const;
    int getStartingMoney() const { return startingMoney; }
    int getNet() const { return money - startingMoney; }  // +gain / -loss

    // Bets
    void setBet(int betAmount);
    int getBet() const;              // made const-safe

    // Outcomes (also clear hand at end of round)
    void handWon(int moneyWon);      // money += moneyWon; ++wins
    void handLost(int moneyLost);    // money -= moneyLost; ++losses
    void handPush();                 // no money change; ++pushes

    // Stats accessors
    int getWins()   const { return wins; }
    int getLosses() const { return losses; }
    int getPushes() const { return pushes; }
};

#endif // PLAYER_H
