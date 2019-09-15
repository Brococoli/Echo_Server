#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 4096

void err_ret(const char*);
void err_quit(const char*);
void err_sys(const char*);
void AddFL(int fd, int arg);
void RemoveFL(int fd, int arg);
#endif
