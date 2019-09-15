#ifndef AUTO_BUFFER_H_
#define AUTO_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

static const int MAXBUFSIZE = 4096;
/*---------------------------------------*/
/*
 *front                     rear
 *  |                        |
 *-----  ------  ------   ----- 
 *|---|->|----|->|----|->|-----|
 *-----  ------  ------  ------
 *  |                     | 
 * front_ptr             rear_ptr
 * 
 */

typedef struct BufferNode{
    char* data_ptr;
    struct BufferNode* next;    
}AutoBufferNode;

typedef struct{
    int front_ptr, rear_ptr;
    AutoBufferNode *front, *rear;
}AutoBuffer;

AutoBuffer* InitBuf();
void FreeBuf(AutoBuffer* buf);
int isBufFull(AutoBuffer* buf);
int isBufEmpty(AutoBuffer* buf);
int ReadFromFile(int fd, AutoBuffer* buf);
int WriteToFile(int fd, AutoBuffer* buf);

#endif
