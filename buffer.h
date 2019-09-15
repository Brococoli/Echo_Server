#ifndef buffer_h
#define buffer_h
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
typedef struct{
    char* ptr;
    int front, rear;
    int size;
} NormalBuffer;

static const int MAXBUFSIZE = 8192;
/* 
 *  ------------------------
 *  ------------------------
 *   -                 -     
 *  rear              front 
 */

NormalBuffer* InitBuf(int size);
void FreeBuf(NormalBuffer* buf);
int isBufFull(NormalBuffer* buf);
int isBufEmpty(NormalBuffer* buf);
int ReadFromFile(int fd, NormalBuffer* buf);
int WriteToFile(int fd, NormalBuffer* buf);
#endif
