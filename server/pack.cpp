#include "pack.h"


void packi16(unsigned char *buf, unsigned int i) {
   *buf++ = i>>8;  *buf++ = i;
}

unsigned int unpacki16(unsigned char *buf) {
   return (buf[0]<<8) | buf[1];
}

int8_t pack(unsigned char *buf,int id,std::vector<int>& cards ) {
    int8_t b;
    char *s;
    int8_t size = 0, len;
    unsigned char *start;

    int num=cards.size();
    // size header
    start = buf;
    *buf++ = size;
    *buf++=id;
    size+=1;

    for(int i=0; i<num; i++)
    {

        size += 1;
        b = (int8_t)cards[i];
        *buf++ = b;
        break;
    }


    *start = size;
    cards.clear();
    // return total packet size (incl. header);
    return size + 1;
}

void unpack(unsigned char *buf,int& id,std::vector<int>& cards) {
    va_list ap;
    int16_t *h;
    int8_t *b;
    char *s;
    int32_t len, count, maxstrlen=0;
    int num=*buf;
    buf++;
    id=*buf;
    for(int i=0;i<num; i++) {

            cards.push_back(*buf++);


    }

}
