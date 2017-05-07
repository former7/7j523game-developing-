#include "seat.h"

seat::seat() {};

seat::seat(int8_t n) {
    seatNumber = n;
    score=0;
    for(int i=0;i<54;i++)
    {
        handCards.insert(make_pair(i,0));
    }
}
void seat:: wins(int n) {
    score+= n;
}


void seat::newRound() {
}

seat::~seat() {}
