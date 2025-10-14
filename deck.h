#ifndef DECK_H
#define DECK_H
#include<vector>
using namespace std;
//Deck class header file
class Deck
{
private:
    vector<int> shoe;
public:
    Deck();
    void shuffle();//will build and shuffle a deck of cards
    int deal();//deals 1 card per call
    vector<int> viewDeck();//will return the current deck
};

#endif // DECK_H
