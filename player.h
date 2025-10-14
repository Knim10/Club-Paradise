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
    int money;
    int bet;
public:
    explicit Player(string inName, int inMoney);
    const string& getName();
    void setBet(int betAmount);//sets bets variable to betAmount
    int getBet();//returns bet
    void handWon(int moneyWon);//adds moneyWon to money player has
    void handLost(int moneyLost);//subtracts moneyLost from money player has
};

#endif // PLAYER_H
