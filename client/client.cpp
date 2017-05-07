#include "client.h"

using namespace std;

client::client(const char *_name) {
   name = _name;
}

client::~client() {
   close(sockfd);
}

int client::connectToServer(const char *remoteHost, const char *port) {
   struct addrinfo hints, *res;
   int status;

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((status = getaddrinfo(remoteHost, port, &hints, &res)) != 0)
      return -1;

   if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
      return -1;

   if((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) != 0)
      return -1;

   return gameLoop();
}

int client::ready()
{
   unsigned char msg[2];
   int msg_len;

   msg_len = pack(msg, "b", 10);
   return sendAll(sockfd, msg, msg_len);
}
// specs in ./list_of_actions
int client::read(unsigned char *buf) {

   unsigned char *p = buf;
   int8_t a, b, c;
   int16_t d, f;
   unsigned char s[64];
   int8_t action = *p++;

   switch(action) {
   case 40:
      placeBet();
      return 0;
   default:
      return -1;
   }

}

int client::gameLoop() {
   unsigned char msg[64];
   int msg_len;

   msg_len = pack(msg, "bs", 0, name);
   sendAll(sockfd, msg, msg_len);


   unsigned char buf[64];
   unsigned char *p;
   unsigned char tmp_buf[130];
   int tmp_offset = 0;
   int buf_len = sizeof buf;
   int bytes_received = 0;
   int8_t size;

   while((bytes_received = recv(sockfd, buf, buf_len, 0)) > 0) {
      
      //fprintf(stdout, "Received %" PRId8 " Bytes\n", bytes_received);

      memcpy(tmp_buf + tmp_offset, buf, bytes_received);
      tmp_offset += bytes_received;

      if(bytes_received < 1)
         continue;

      size = *tmp_buf;

      while(tmp_offset >= size + 1) {
         //fprintf(stdout, "Read %" PRId8 " Bytes\n", size + 1);

         p = tmp_buf + 1;
         read(p);

         tmp_offset -= (size + 1);

         memmove(tmp_buf, tmp_buf + size + 1, tmp_offset);

         if(tmp_offset > 0)
            size = *tmp_buf;
      }
   }

   return 0;
}

int client::sendAll(int fd, unsigned char *msg, int msg_len) {
   int bytes_left = msg_len;
   int bytes_send = 0;
   while(bytes_left > 0) {
      if((bytes_send = send(fd, msg + msg_len - bytes_left, bytes_left, 0)) == -1)
         return -1;
      bytes_left = bytes_left - bytes_send;
   }
   return 0;
}
