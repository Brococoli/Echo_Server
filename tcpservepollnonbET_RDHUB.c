#include <stdio.h>
#include "auto_buffer.h"
#include "utils.h"
#include "agent.h"
#include "socket_utils.h"
#include <sys/epoll.h>
#include <assert.h>

#define MAXEVENT 1024
#define BUFSIZE 4096

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, connfd;
    int i;
    socklen_t clilen;

    int ndfs, epfd;


    listenfd = ExamError(socket(AF_INET, SOCK_STREAM, 0), 
                          "socket error", 0); 

    SetSocketAddr(&servaddr, AF_INET, INADDR_ANY, 9888);

    ExamError(bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)),
              "bind error", 0);

    ExamError(listen(listenfd, LISTENQ), "listen error", 0);

    struct epoll_event ev, events[MAXEVENT];
    epfd = epoll_create1(0);

    AddFL(listenfd, O_NONBLOCK);

    ev.data.ptr = CreateAgent(listenfd, 0);
    ev.events  = EPOLLIN | EPOLLET;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
        err_sys("epoll_ctl error");


    for(;;){
        ndfs = epoll_wait(epfd, events, MAXEVENT, -1);
        /* fprintf(stdout, "nfds: %d\n", ndfs); */
        if(ndfs < 0)
            err_sys("epoll_wait error");

        for(i = 0; i < ndfs; ++i){
            uint32_t event = events[i].events;
            int fd = getFd((Agent*)events[i].data.ptr);
            /* fprintf(stdout, "fd: %d, event: %s, %s, %s\n", fd, (event&EPOLLIN)?"EPOLLIN":"NOTIN", (event&EPOLLOUT)?"EPOLLOUT":"NOTOUT", (event&EPOLLRDHUP)?"EPOLLRDHUP": "NOEPOLLRDHUB"); */
            if(fd == listenfd){
                clilen = sizeof(cliaddr);
                while( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) > 0){
                    /* fprintf(stderr, "connect %d\n", connfd); */
                    AddFL(connfd, O_NONBLOCK);
                    events[i].events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLOUT;
                    events[i].data.ptr = CreateAgent(connfd, BUFSIZE);
                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, events + i);
                }
                if(connfd < 0){
                    if(errno != EWOULDBLOCK)
                        err_sys("Accept error");
                }

            }

            else{
                Agent* agent = (Agent*)events[i].data.ptr;

                if(event & EPOLLIN){
                    ReadData(agent, 0);
                }
                if(event & EPOLLOUT){
                    if(canWriteData(agent)) WriteData(agent, 0);
                }
                if(event & EPOLLRDHUP){ //可能是对端写关闭或者链接中断
                    int finish = 0;
                    int error;
                    socklen_t len = sizeof(error);

                    error = 0;
                    getsockopt(agent->fd, SOL_SOCKET, SO_ERROR, &error, &len);
                    if(error != 0){ //发生错误：对端已经关闭，或者链接中断
                        err_sys("connect errror");
                        finish = 1;
                    }
                    else{ //链接正常
                        shutdown(fd, SHUT_RD);
                        if(hasReadEOF(agent) && !canWriteData(agent)) finish = 1; 
                    }

                    if(finish){
                        /* fprintf(stderr, "close %d\n", fd); */
                        close(fd);
                        DeleteAgent((Agent*)events[i].data.ptr);
                        epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
                    }
                }
                /* sleep(5); */
            }
        }
    }
    return 0;
}
