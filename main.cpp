/*
* idcoddington@dmacc.edu isaac coddington
* 9/28/25
* 
* Kenneth Nimmo Knimmo1@dmacc.edu
* 10/2/2025
* 
* comment/uncomment test cases as needed
* 
* midterm group project
*/
#include "deck.h"
#include "player.h"
#include "table.h"
#include<iostream>
using namespace std;

int main()
{
    /*
    //variable set up
    Deck cards;
    string name;
    int money;
    int card;
    cards.shuffle();
    cout << "Name\n";
    getline(cin, name);
    cout << "Money\n";
    cin >> money;
    //create player and simulating cards dealt
    Player player1(name, money);
    card = cards.deal();
    cout << "Card dealt to " << name << " " << card;
    player1.cardDealt(card);
    card = cards.deal();
    cout << "\nNext card dealt to " << name << " " << card << endl;
    player1.cardDealt(card);
    cout << name << "'s hand ";
    player1.showHand();
    //simulating placing a bet, winning, and losing
    cout << endl << name << " place your bet: ";
    cin >> money;

    player1.setBet(money);
    cout << "Emulating bet won " << name << "'s bank is now " << player1.handWon(player1.getBet());
    cout << "\nEmulating bet lost " << name << "'s bank is now " << player1.handLost(player1.getBet());
    */

    // table test cases
    Deck deck;
    deck.shuffle();

    Player alice("Alice", 1000);
    Player bob("Bob", 1000);

    Table table(deck);
    table.addPlayer(&alice);
    table.addPlayer(&bob);

    // -------------------------------
    // Test Case 1: Player beats dealer
    // Alice: 19, Bob: 16, Dealer: 17 (stands) -> Alice win, Bob loss
    // -------------------------------
    cout << "\n=== Table Test Case 1: Player beats dealer ===\n";
    alice.setBet(100);
    bob.setBet(100);

    // clear everyone
    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(9);   // 19
    bob.cardDealt(8);    bob.cardDealt(8);     // 16

    // dealer's hand (hard-code to 17)
    table.testClearDealer();
    table.testDealToDealer(10);
    table.testDealToDealer(7);                 // 17

    // no need to call dealerPlay(); dealer is already set
    table.settleBets();

    // -------------------------------
    // Test Case 2: Dealer busts
    // Alice: 17, Bob: 16, Dealer: 24 (bust) -> both win
    // -------------------------------
    cout << "\n=== Table Test Case 2: Dealer busts ===\n";
    alice.setBet(50);
    bob.setBet(50);

    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(7);   // 17
    bob.cardDealt(9);    bob.cardDealt(7);     // 16

    // dealer's hand (hard-code to a bust total)
    table.testClearDealer();
    table.testDealToDealer(10);
    table.testDealToDealer(6);
    table.testDealToDealer(8);                 // 24 -> bust

    table.settleBets();

    // -------------------------------
    // Test Case 3: Push
    // Alice: 17, Bob: 17, Dealer: 17 -> both push
    // -------------------------------
    cout << "\n=== Table Test Case 3: Push ===\n";
    alice.setBet(25);
    bob.setBet(25);

    alice.clearHand();
    bob.clearHand();
    table.clearHands();

    // players' hands
    alice.cardDealt(10); alice.cardDealt(7);   // 17
    bob.cardDealt(9);    bob.cardDealt(8);     // 17

    // dealer's hand (hard-code to 17)
    table.testClearDealer();
    table.testDealToDealer(9);
    table.testDealToDealer(8);                 // 17

    table.settleBets();

    cout << "\n=== Table Tests complete ===\n";
    return 0;

}
