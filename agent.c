#include "agent.h"
void AddAgentBuf(Agent* agent, int size){
    if((agent->buf = InitBuf(size)) == NULL)
            fprintf(stderr, "AddAgentBuf error");
}
Agent* CreateAgent(int fd, int size){
    Agent* agent = (Agent*)malloc(sizeof(Agent));
    if(agent == NULL)
        err_sys("CreatFdDatai: malloc error");
    AddAgentBuf(agent, size);
    agent->fd = fd;
    agent->eof = 0;
    return agent;

}
void RemoveAgentBuf(Agent* agent){
    if(agent->buf) 
        FreeBuf(agent->buf);
}
int DeleteAgent(Agent* agent){
    RemoveAgentBuf(agent);
    return 1;
}
int getFd(Agent* agent){
    return agent->fd;
}
int hasReadEOF(Agent* agent){
    return agent->eof;
}
int canReadData(Agent* agent){
    return !isBufFull(agent->buf);
}
int canWriteData(Agent* agent){
    return !isBufEmpty(agent->buf);
}
int ReadData(Agent* agent, int block){
    if(block) return ReadData_block(agent);
    else return ReadData_nonb(agent);
}
int WriteData(Agent* agent, int block){
    if(block) return WriteData_block(agent);
    else return WriteData_nonb(agent);
}
int ReadData_block(Agent* agent){
    if(agent == NULL || !canReadData(agent)) return -1;
    int ret = ReadFromFile(agent->fd, agent->buf); 
    if(ret == 0) agent->eof = 1;
    return ret;
}
int WriteData_block(Agent* agent){
    if(agent == NULL || !canWriteData(agent)) return -1;
    return WriteToFile(agent->fd, agent->buf);
}
int WriteData_nonb(Agent* agent){
    int n;

    while( (n = WriteToFile(agent->fd, agent->buf)) > 0);
    
    if(n < 0){
        if(errno != EWOULDBLOCK)
            err_sys("WriteData_nonb: WriteToFile: write error");
        return EWOULDBLOCK;
    }

    return 0;
}
int ReadData_nonb(Agent* agent){
    int n;

    while ( (n = ReadFromFile(agent->fd, agent->buf)) > 0);

    if(n < 0){
        if(errno != EWOULDBLOCK)
            err_sys("ReadData_nonb: ReadFromFile: read error");
        return EWOULDBLOCK;
    }
    else if(!isBufFull(agent->buf)) {
        agent->eof = 1; //若buf空且返回n=0说明读到eof了
        return EOF;
    }
    return EBUFFULL; //缓存区满了，但是数据还没有读完 */
}
