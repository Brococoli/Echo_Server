#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include <stdarg.h>

#define LISTENQ 2046
typedef struct sockaddr SA;
char * Sock_ntop(const struct sockaddr* sa, socklen_t salen);
void SetSocketAddr(void* sockaddr, int family, int ip_addr, int port);

int ExamError(int status,const char* msg, int errnum, ... /*ignore error number*/);
#endif
