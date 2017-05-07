#ifndef PACK_H
#define PACK_H

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include<vector>

void packi16(unsigned char *buf, unsigned int i);

unsigned int unpacki16(unsigned char *buf);

int8_t pack(unsigned char *buf, int num,int id,std::vector<int>& cards ); 

void unpack(unsigned char *buf,int& id,std::vector<int>& cards); 
#endif
