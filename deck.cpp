#include "deck.h"
#include<algorithm>
#include<random>
Deck::Deck() : shoe() {}
//fills and shuffle a vector with card values 2-11
void Deck::shuffle() {
    shoe.clear(); //clear vector if you need to reshuffle and the vector isn't empty
    //insert values 2-9 with for loops
    for (int cardValue = 2; cardValue <= 9; cardValue++) {
        for (int i = 0; i < 4; i++) {
            shoe.push_back(cardValue);
        }
    }
    //insert 10 values
    for (int i = 0; i < 16; i++) {
        shoe.push_back(10);
    }
    //insert 11 values for aces
    for (int i = 0; i < 4; i++) {
        shoe.push_back(11);
    }
    //I found this code from a stack overflow page here "https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector"
    //The first line grabs a rng engine, and the second line is a shuffle function using the rng engine
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(shoe.begin(), shoe.end(), rng);
}
//this will deal 1 card per call, if the shoe vector is empty it will refill, shuffle, then deal
int Deck::deal() {
    if (shoe.empty()) {
        shuffle();
        int card = shoe.back();
        shoe.pop_back();
        return card;
    }
    else {
        int card = shoe.back();
        shoe.pop_back();
        return card;
    }
}
//will return the current shoe, if shoe is empty will return a full shuffled shoe
vector<int> Deck::viewDeck() {
    if (shoe.empty()) {
        shuffle();
        return shoe;
    }
    else {
        return shoe;
    }
}
