#include <stdio.h>
#include "buffer.h"
#include "utils.h"
#include "agent.h"
#include "socket_utils.h"
#include <sys/epoll.h>
#include <assert.h>
#include <pthread.h>

#define MAXEVENT 1024
#define MAXEPOLLEVENT 1000
#define BUFSIZE 4096
typedef struct {
    int epfd;
    int fd_count;
    int finish;
    pthread_mutex_t mutex_fd_count;
}EpollData;

void* epoll_deal(void* arg){
    int* fd_count = &((EpollData*)arg)->fd_count;
    int epfd = ((EpollData*)arg)->epfd;
    int* fd_finish = &((EpollData*)arg)->finish;
    pthread_mutex_t* mutex_fd_count = &((EpollData*)arg)->mutex_fd_count;
    int ndfs,i;
    struct epoll_event events[MAXEVENT];
    for(;;){
        ndfs = epoll_wait(epfd, events, MAXEVENT, -1);
        fprintf(stdout, "epfd: %d, nfds: %d\n", epfd, ndfs);
        if(ndfs < 0){
            err_sys("epoll_wait error");
        }

        for(i = 0; i < ndfs; ++i){
            uint32_t event = events[i].events;
            int fd = getFd((Agent*)events[i].data.ptr);
            fprintf(stdout, "epfd: %d, fd: %d, event: %s, %s, %s\n", epfd, fd, (event&EPOLLIN)?"EPOLLIN":"NOTIN", (event&EPOLLOUT)?"EPOLLOUT":"NOTOUT", (event&EPOLLRDHUP)?"EPOLLRDHUP": "NOEPOLLRDHUB");

            Agent* agent = (Agent*)events[i].data.ptr;

            if(event & EPOLLIN){
                int buf_full = isBufFull(agent->buf);
                if(!buf_full){
                    int ret = ReadData(agent, 0);
                    if(ret == EOF){
                    fprintf(stderr, "read eof\n");
                    }
                    if(ret == EBUFFULL)  buf_full = 1;
                }
                if(buf_full){ //若新消息来了，buf还是满的也要重新MOD
                    events[i].events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLOUT;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, events + i);
                }
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
                    fprintf(stderr, "close %d\n", fd);
                    close(fd);
                    DeleteAgent((Agent*)events[i].data.ptr);
                    epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
                    if(*fd_finish != 1)
                        pthread_mutex_lock(mutex_fd_count);
                    (*fd_count)--;
                    fprintf(stdout, "fd_count: %d\n", *fd_count);
                    if(*fd_finish == 1 && *fd_count == 0){
                        close(epfd);
                        fprintf(stderr, "close thread: %ld\n", pthread_self());
                        free(arg);
                        pthread_exit(NULL);
                    }
                    else 
                        if(*fd_finish != 1)
                            pthread_mutex_unlock(mutex_fd_count);
                }
            }
        }
    }
}

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, connfd;
    int i;
    socklen_t clilen;

    int ndfs, listen_epfd;


    listenfd = ExamError(socket(AF_INET, SOCK_STREAM, 0), 
                         "socket error", 0); 

    SetSocketAddr(&servaddr, AF_INET, INADDR_ANY, 9888);

    ExamError(bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)),
              "bind error", 0);

    ExamError(listen(listenfd, LISTENQ), "listen error", 0);

    struct epoll_event ev, events[MAXEVENT];
    listen_epfd = epoll_create1(0);

    AddFL(listenfd, O_NONBLOCK);
    ev.data.ptr = CreateAgent(listenfd, 0);
    ev.events  = EPOLLIN | EPOLLET;

    if(epoll_ctl(listen_epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
        err_sys("epoll_ctl error");


    EpollData* epoll_data = NULL;
    pthread_t tid;
    for(;;){
        ndfs = epoll_wait(listen_epfd, events, MAXEVENT, -1);
        fprintf(stdout, "epfd: %d, nfds: %d\n", listenfd, ndfs);
        if(ndfs < 0)
            err_sys("epoll_wait error");

        for(i = 0; i < ndfs; ++i){
            uint32_t event = events[i].events;
            int fd = getFd((Agent*)events[i].data.ptr);
            fprintf(stdout, "epfd: %d, fd: %d, event: %s, %s, %s\n", listenfd, fd, (event&EPOLLIN)?"EPOLLIN":"NOTIN", (event&EPOLLOUT)?"EPOLLOUT":"NOTOUT", (event&EPOLLRDHUP)?"EPOLLRDHUP": "NOEPOLLRDHUB");
            if(fd == listenfd){
                clilen = sizeof(cliaddr);
                while( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) > 0){
                    if(connfd < 0){
                        if(errno != EWOULDBLOCK)
                            err_sys("Accept error");
                    }
                    fprintf(stderr, "connect %d\n", connfd);
                    if(epoll_data == NULL){
                        epoll_data = (EpollData*)malloc(sizeof(EpollData));
                        epoll_data->epfd = epoll_create1(0);
                        epoll_data->finish = 0;
                        pthread_mutex_init(&epoll_data->mutex_fd_count, NULL);
                        epoll_data->fd_count = 0;
                        errno = pthread_create(&tid, NULL, epoll_deal, epoll_data);
                        fprintf(stderr, "thread create: %ld\n", tid);
                        if(errno != 0)
                            err_sys("pthread_create error");
                        errno = pthread_detach(tid);
                        if(errno != 0)
                            err_sys("pthread_detach error");
                    }
                    AddFL(connfd, O_NONBLOCK);
                    events[i].events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLOUT;
                    events[i].data.ptr = CreateAgent(connfd, BUFSIZE);
                    epoll_ctl(epoll_data->epfd, EPOLL_CTL_ADD, connfd, events + i);
                    pthread_mutex_lock(&epoll_data->mutex_fd_count);
                    epoll_data->fd_count ++ ;
                    if(epoll_data->fd_count == MAXEPOLLEVENT){
                        epoll_data->finish = 1;
                        pthread_mutex_unlock(&epoll_data->mutex_fd_count);
                        epoll_data = NULL;
                    }
                    else
                        pthread_mutex_unlock(&epoll_data->mutex_fd_count);

                }

            }
        }
    }
    return 0;
}
