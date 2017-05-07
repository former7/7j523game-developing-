#include "pack.h"
#include "deckC.h"
#include "socketHandler.cpp"
#include "seat.h"
#include "handEvaluator.h"
#include "log.h"

#include <cstdlib>
#include <time.h>

class server {

public:

    server(const char *port, int playerCount);

    int startGame();

    ~server();

private:

    int sendAll(int fd, unsigned char *msg, int msg_len);

    int broadcast(unsigned char *msg, int msg_len);

    int playerWon(int8_t n, int16_t amount);

    int readNext(int8_t n);

    int clear(int8_t n);

    int gameLoop();

    int waitForNextRound();
 
    int playerCount;
    CLog log;

    const char *port;
    int handCardNum; 
    seat player[8];
    int playersLeft;
    int lastPlayer; 
    int serverId;
    int tableScore;
    enum STATUS(NORMAL,ERROR,WARNING,END);
    STATUS state;
};
