#include <inttypes.h>
#include <cstdlib>
#include <time.h>

class deckC {

public:
   deckC();

   int8_t getCard();
   
   void shuffle();

   int getLeftCardNum(){return cardsLeft;}
private:
   int8_t cards[52];
   int cardsLeft;

};
