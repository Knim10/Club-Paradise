/*
 * isaac coddington 9/30
*/
#ifndef PERSON_H
#define PERSON_H
#include <vector>
using namespace std;

class Person {
protected:
    vector<int> hand;
    bool ace = false;
public:
    virtual ~Person() {}
    void cardDealt(int card);
    int handValue();
    void showHand();
    void clearHand();
};

#endif // PERSON_H

