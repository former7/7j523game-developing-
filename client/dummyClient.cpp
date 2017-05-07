#include <fstream>
#include <cstring>

#include "client.h"

using namespace std;

class dummyClient : public client {

public:
   dummyClient(const char *name) : client(name) {};
   ~dummyClient() {};

protected:
   void placeBet();

   void nextRound();

   void playerWon(int8_t n, int16_t amount);
private:
   string getCardName(int8_t c);
};


void dummyClient::placeBet() {
   check();
}

void dummyClient::nextRound() {
   fprintf(stdout, "Next Round\n");
   ready();
}


void dummyClient::playerWon(int8_t n, int16_t amount) {
   fprintf(stdout, " Player %d won %d\n", n, amount);
}


int main(int argc, char *argv[]) {
   const char *port = "3031";
   const char *ip = "127.0.0.1";

   if(argc > 1)
      ip = argv[1];
   if(argc > 2)
      port = argv[2];

   fprintf(stdout, "Connection to %s on %s\n", ip, port);

   dummyClient scn("scn");

   scn.connectToServer(ip, port);

}
