#include "utils.h"

void AddFL(int fd, int arg){
    int val = fcntl(fd, F_GETFL, 0);
    if(fcntl(fd, F_SETFL, val | arg) < 0)
        err_sys("AddFL error");
}
void RemoveFL(int fd, int arg){
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0)&(~arg)) < 0)
        err_sys("RemoveFL error");
}

void err_ret(const char* msg){
    perror(msg);
    return;
}
void err_quit(const char* msg){
    perror(msg);
    exit(-1);
}
void err_sys(const char* msg){
    err_quit(msg);
}
