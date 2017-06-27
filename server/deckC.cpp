#include "deckC.h"

deckC::deckC() {
   // init rng
   time_t seconds;
   time(&seconds);
   srand((unsigned int) seconds);
}

void deckC::shuffle() {
   cardsLeft = 54;
   for(int8_t i = 0; i < 54; i++)
      cards[i] = i;
}

int8_t deckC::getCard() {
    if(cardsLeft<=0)
        return -1;
    int index = rand() % cardsLeft;
    int8_t card = cards[index];
    for(int i = index; i < cardsLeft - 1; i++)
        cards[i] = cards[i+1];

    cardsLeft--;
    return card;
}
