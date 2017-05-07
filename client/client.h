#ifndef CLIENT_H
#define CLIENT_H

#include "pack.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//#include <fstream>
 
class client {
 
public:
 
   client(const char *name);

   int connectToServer(const char *remoteHost, const char *port);

   ~client();

protected:

   // communication to server
   int ready();

   /* 
      communication from server
      gets called when server requests your action
      answer with check() call() raise()...
   */

   virtual void nextRound() { ready(); }; // tell server when you are ready 45s timeout
   // hooks for notifications from server

   virtual void playerWon(int8_t n, int16_t amount) {};

   virtual void setPlayerName(int8_t n, unsigned char *name) {};

   virtual void playerLeft(int8_t n) {};

   virtual void actionAccepted() {};

   virtual void actionUnknown() {};

   virtual void actionNotAllowed() {};

private:

   const char *name;

   int sockfd;

   int gameLoop();

   int read(unsigned char *buf);

   int sendAll(int fd, unsigned char *msg, int msg_len);
};

#endif
