#include "server.h"
#include "command.h"
using namespace std;

server::server(const char *port, int playerCount) {
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);
    handCardNum=5;
    serverId=99;
    this->port = port;
    this->playerCount = playerCount;
    playersLeft = playerCount;


    for(int8_t i = 0; i < playerCount; i++)
        player[i] = seat(i, startingChips);

    log.setLogLevel(VERBOSE);
    log.setDisplayMessages(true);

}

server::~server() {
   int client[playerCount];
   for(int i = 0; i < playerCount; i++)
      client[i] = player[i].getFD();
   cleanup(client);
}

int server::sendAll(int n, unsigned char *msg, int msg_len) {
   int fd = player[n].getFD();
   int bytes_left = msg_len;
   int bytes_send = 0;
   while(bytes_left > 0) {
      if((bytes_send = send(fd, msg + msg_len - bytes_left, bytes_left, 0)) == -1)
         return -1;

      bytes_left = bytes_left - bytes_send;
   }
   return 0;
}

int server::broadcast(unsigned char *msg, int msg_len) {
   for(int i = 0; i < playerCount; i++) {
      if(sendAll(i, msg, msg_len) == -1) {
         return -1;
      }
   }
   return 0;
}

int server::waitForNextRound() {
    struct timeval tv;
    fd_set readfds, master;

    time_t start, now;

    unsigned char buf[32], tmp_buf[64], *p;
    int buf_len = sizeof buf;
    int tmp_offset = 0;
    int bytes_received = 0;
    int8_t size, action;
    bool noready=true;
    unsigned char msg[16];
    int msg_len, maxFD;

    int playersReady = 0;

   // ask players
    log.log(VERBOSE, "Waiting for players");
    msg_len = pack(msg, "b", 19);
    for(int i = 0; i < playersLeft; i++)
        if(sendAll(i, msg, msg_len) == -1)
            log.log(ERROR, "ERROR: asking %d if ready", i);


    FD_ZERO(&readfds);
    FD_ZERO(&master);
    maxFD = -1;

    for(int i = 0; i < playersLeft; i++) {
        FD_SET(player[i].getFD(), &master);
        if(player[i].getFD() > maxFD)
            maxFD = player[i].getFD();
    }

    time(&start);
    time(&now);

    while(difftime(now, start) < 45 && playersReady < playerCount) {
        tv.tv_sec = 45 - difftime(start, now);
        tv.tv_usec = 0;

        readfds = master;

        if(select(maxFD + 1, &readfds, NULL, NULL, &tv) == -1)
            log.log(ERROR, "ERROR: selecting");


        for(int i = 0; i < maxFD + 1; i++) {
            noready=true;
            if(FD_ISSET(i, &readfds)) {
                while(noready){
             
                    if((bytes_received = recv(i, buf, buf_len, 0)) > 0) {

                        memcpy(tmp_buf + tmp_offset, buf, bytes_received);
                        tmp_offset += bytes_received;
                        if(bytes_received < 1)
                            continue;

                        size = *tmp_buf;

                        if(tmp_offset >= size + 1) {

                        p = tmp_buf + 1;

                        action = *p++;

                        switch(action) {
                        case 10:
                            FD_CLR(i, &master);
                            playersReady++;
                            noready=false;
                            log.log(VERBOSE, "socket %d ready", i);
                            break;
                        }

                        tmp_offset -= (size + 1);
                        memmove(tmp_buf, tmp_buf + size + 1, tmp_offset);

                        if(tmp_offset > 0)
                            size = *tmp_buf;
                        }
                    }
                }   
            }
        }
        time(&now);
    }

    return -1;
}

int server::readNext(int8_t n,int &id,vector<int>& cards) {

    struct timeval tv;
    fd_set readfds;

    time_t start, now;

    int fd = player[n].getFD();
    unsigned char buf[32], tmp_buf[64], *p;
    int buf_len = sizeof buf;
    int tmp_offset = 0;
    int bytes_received = 0;
    int8_t size, action;
    int16_t d;

    unsigned char msg[16];
    unsigned char s[16];
    int msg_len;
    time(&start);
    time(&now);

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    while(difftime(now, start) < 45) {

        tv.tv_sec = 45 - difftime(start, now);
        tv.tv_usec = 0;
        select(fd+1, &readfds, NULL, NULL, &tv);

        if(FD_ISSET(fd, &readfds)) {
            if((bytes_received = recv(fd, buf, buf_len, 0)) > 0) {

                memcpy(tmp_buf + tmp_offset, buf, bytes_received);
                tmp_offset += bytes_received;
                if(bytes_received < 1)
                    continue;

                size = *tmp_buf;

                if(tmp_offset >= size + 1) {

                    p = tmp_buf + 1;

                    action = *p++;

                    switch(action) {
                        //case NAME:
                        //    unpack(p, "s", &s);
                        //    player[n].setName(s);
                        //    msg_len = pack(msg, "bbs", 11, player[n].getNumber(), s);
                        //    if(broadcast(msg, msg_len) == -1)
                        //        log.log(ERROR, "ERROR: broadcasting name");
                  
                        //    log.log(SERVER, "%s joined", player[n].getName());
                        //    return 1;
                        case ABNDON:
                            return -1 //player不出
                        default:
                            unpack(p,id,cards);
                             return 0;
                    }

                    tmp_offset -= (size + 1);
                    memmove(tmp_buf, tmp_buf + size + 1, tmp_offset);

                    if(tmp_offset > 0)
                        size = *tmp_buf;
                }
            }
        }
        time(&now);
    }

   return -1;

}


int server::startGame() {
    int client[playerCount];

    if((setup(client, playerCount, port)) == -1) {
        log.log(ERROR, "ERROR: socket error");
            return -1;
    }

    for(int i = 0; i < playerCount; i++)
        player[i].setFD(client[i]);

   
   // get names
    for(int i = 0; i < playerCount; i++) {
        if(readNext(i) == -1)
            log.log(ERROR, "ERROR: reading names");
    }


    // broadcast playerCount
    unsigned char msg[64];
    int msg_len;
    msg_len = pack(msg, "bb", 10, playerCount);
    if(broadcast(msg, msg_len) == -1)
      log.log(ERROR, "ERROR: broadcasting playercount");


   


   // assign players their seat and tell others
    for(int i = 0; i < playerCount; i++) {
      msg_len = pack(msg, "bb", 12, player[i].getNumber());
      if(sendAll(i, msg, msg_len) == -1)
         log.log(ERROR, "ERROR: assigning seat to %d", i);

    }

   // broadcast starting chips
 //  msg_len = pack(msg, "bh", 15, startingChips);
 //  if(broadcast(msg, msg_len) == -1)
 //     log.log(ERROR, "ERROR: broadcasting starting chips");

   // commence
   return gameLoop();
   
}

int server::dealCards(deckC& deck,vector<int> playerLive)
{   
    int dealNum=0;
    bool left=true;
    for(int i=0;i<playerCount;i++)
    {
        if(playerLive[i]==1)
        {
           
            unsigned char msg[16];
            dealNum=handCardNum-seat[i].getHandNum();
            if(dealNum<=0)continue;
            int j=dealNum;
            vector<int> genCards;
            //generate cards
            while(j>0)
            {
                int card=deck.getCard();
                if(card==-1)
                    left=false;
                else
                {
                    genCards.push_back(card);
                    player[i].handCards[card]++;
                }   
                    
                j--;
            }
            
            log.log(VERBOSE,cards,"player%d get cards: ",i);
            //pack cards
            int len=pack(msg,DEALCARD,genCards);
            //send cards
            sendAll(i,msg,len);                      
            if(!left)return -1;
        }
    }
    
}
void server::calTableScore(vector<int> &cards)
{
    for(int i=0;i<cards.size();i++)
    {
        if(cards[i]%13==3)
            tableScore+=5;
        else if(cards[i]%13==8)
            tableScore+=10;
        else if(cards[i]%13==11)
            tableScore+=10;
    }
}
int server::waitCards(deckC& deck,vector<int>& playerLive);                        
{
    for(int i=0;i<playerCount;i++)
    {
        if(playerLive[i]==1)
        {
            unsigned char msg[16];
            vector<int>cards;
            int res;
            if(readNext(i,cards)==-1)
            {
                playerLive[i]=0;
                playerLeft--;
                log.log(VERBOSE,"player%d give up ",i);
                continue;
            }
            else
            {
                //calculate table score
                calTableScore(card);
                for(int j=0;j<cards.size();j++)
                {
                    player[i].handCards[card[j]]--;
                }
                int len=pack(msg,i,cards);
                
                log.log(VERBOSE,cards,"player%d play cards: ",i);
                //notify all players
                broadcast(msg,len);
                if(deck.cardsLeft<1)
                {
                    lastPlayer=i;
                    state=END;
                    log.log(VERBOSE,"cards left 0");
                    return -1;
                }
            }
        }
    }
    return 0;
}
void calResult(vector<int>& playerLive)
{
    if(lastPlayer>=0)
    {
        player[lastPlayer].score+=tableScore;
        log.log(VERBOSE,"player%d win %d score",lastPlayer,tableScore);
       return;
    }
    for(int i=0;i<playerCount;i++)
    {
        if(playerLive[i]==1)
        {
            player[i].score+=tableScore;
        }
        log.log(VERBOSE,"player%d win %d score",i,tableScore);
    }
} 
void broadcastScore()
{
    char *msg;
    vector<int> playerScore;
    for(int i=0;i<playercount;i++)
    {
        playerScore.push_back(player[i].score);
    }
    int len=pack(msg,BROSCORE,playerScore);
    broadcast(msg,len);
}      
int server::gameLoop() {
    int msg_len;
    int round=1;
    state=NORMOL;
    lastPlayer=-1;
    vector<int>playerLive;
    for(int i=0;i<playerCount;i++)
    {
        playerLive.push_back(1);
    }
    deckC deck;
    // shuffle deck
    deck.shuffle();
    
    while(state==NORMAL)
    { 
        for(int i=0;i<playerCount;i++)
        {
            playerLive[i]=1;
        }
        tableScore=0;
        log.log(VERBOSE,"VERBOSE:now start round %d",round);
            // reset seats
        
        log.log(VERBOSE,"wait for players ready");
       // for(int i = 0; i < playersLeft; i++)
       //     player[i].newRound();
            // wait for players to be ready
        waitForNextRound();
            //deal cards
        log.log(VERBOSE,"deal cards");
        if(dealCards(deck,playerLive)==-1)
            state=END;
          
        while(playersLeft > 1)
        {
            //receive cards

            log.log(VERBOSE,"wait for next turn");
            if(waitCards(deck,playerLive)<0)
                break;
        }
        calResult();//计算结果
        broadcastScore();//广播结果
        round++;
    }
    if(state==END)
    {
        char* msg;
        int len=pack(msg,BROEND,playerScore);
        log.log(VERBOSE,"game over,final")
        for(int i=0;i<playerCount;i++)
        {
            log.log(VERBOSE,"player%d score:%d",i,player[i].score);
        }
        broadcast(msg,len);
    }

    return 0;

}


int server::clear(int8_t n) {
   int fd = player[n].getFD();
   fd_set readfds;
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 0;

   FD_ZERO(&readfds);
   FD_SET(fd, &readfds);
   bool empty = false;

   while(!empty) {
      empty = true;
      select(fd+1, &readfds, NULL, NULL, &tv);
      if(FD_ISSET(fd, &readfds)) {
         recv(fd, NULL, 64, 0);
         empty = false;
      }
   }
   return 0;
}


int server::playerWon(int8_t n, int16_t amount) {
   log.log(POT, "Player %d wins %d", player[n].getNumber(), amount);
   player[n].wins(amount);

   unsigned char msg[5];
   int msg_len = pack(msg, "bbh", 52, player[n].getNumber(), amount);
   broadcast(msg, msg_len);

   return 0;
}
}

int main(int argc, char *argv[]) {

   const char *port = "3031";
   int playerCount = 3;

   // read commandline
   for(int i = 1; i < argc; i+=2) {
      if(strcmp(argv[i], "-n") == 0) {
         playerCount = atoi(argv[i+1]);
         if(playerCount > 8 || playerCount < 2) {
            playerCount = 3;
         }
      }
   }

   server myServer(port, playerCount);

   myServer.startGame();
   
}
