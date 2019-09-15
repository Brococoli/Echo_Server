#ifndef BUFFER_H_
#define BUFFER_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

static const int MAXBUFSIZE = 4096;

/* 
 *  ------------------------
 *  ------------------------
 *   -                 -     
 *  rear              front 
 */
typedef struct{
    char* ptr;
    int front, rear;
    int size;
} NormalBuffer;

NormalBuffer* InitBuf(int size);
void FreeBuf(NormalBuffer* buf);
int isBufFull(NormalBuffer* buf);
int isBufEmpty(NormalBuffer* buf);
int ReadFromFile(int fd, NormalBuffer* buf);
int WriteToFile(int fd, NormalBuffer* buf);

#endif // BUFFER_H_

