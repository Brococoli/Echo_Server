#ifndef agent_h
#define AGENT_H
#include "auto_buffer.h"
#include "socket_utils.h"
#include "utils.h"
#include <assert.h>

typedef struct {
    int fd;
    int eof;
    AutoBuffer* buf;
}Agent;

#define EBUFFULL 0

void AddAgentBuf(Agent* agent, int size);
Agent* CreateAgent(int fd, int size);
void RemoveAgentBuf(Agent* agent);
int DeleteAgent(Agent* agent);
int getFd(Agent* agent);

int hasReadEOF(Agent* agent);

int canReadData(Agent* agent);
int canWriteData(Agent* agent);

int ReadData(Agent* agent, int block);
int WriteData(Agent* agent, int block); 

int ReadData_nonb(Agent* agent); //nonblock
int WriteData_nonb(Agent* agent); //nonblock

int ReadData_block(Agent* agent); //block
int WriteData_block(Agent* agent); //block

#endif
