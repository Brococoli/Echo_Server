#include "socket_utils.h"

char * Sock_ntop(const struct sockaddr* sa, socklen_t salen){
    /* 实现了无关协议的ntop */
    char portstr[8];
    static char str[128];

    switch(sa->sa_family){

    case AF_INET:  {
                       struct sockaddr_in* sin = (struct sockaddr_in*) sa;
                       if(inet_ntop(sin->sin_family, &sin->sin_addr, str, sizeof(str)) == NULL)
                           return NULL;

                       if(ntohs(sin->sin_port) != 0){
                           snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                           strcat(str, portstr);
                       }
                       return str;
                   }

    case AF_INET6: {
                       struct sockaddr_in6* sin6 = (struct sockaddr_in6*) sa;
                       if(inet_ntop(AF_INET6, &sin6->sin6_addr, str,  sizeof(sin6->sin6_addr)) == NULL)
                           return NULL ;

                       if(ntohs(sin6->sin6_port) !=0) {
                           snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin6->sin6_port));
                           strcat(str, portstr);
                       }
                       return str;

                   }
    default :{
                 errno = EAFNOSUPPORT;
                 return NULL;

             }
    }
    return NULL;
}
void SetSocketAddr(void* sockaddr, int family, int ip_addr, int port){

    switch(family){

    case AF_INET :{
                      struct sockaddr_in *sockaddr_in = (struct sockaddr_in*) sockaddr;
                      bzero(sockaddr_in, sizeof(*sockaddr_in));
                      sockaddr_in->sin_port = htons(port);
                      sockaddr_in->sin_family = family;
                      sockaddr_in->sin_addr.s_addr = htonl(ip_addr);
                      break;
                  }

                  /*     case AF_INET6 :{ */
                  /*                       struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6*) sockaddr; */
                  /*                       bzero(sockaddr_in6, sizeof(*sockaddr_in6)); */
                  /*                       sockaddr_in6->sin6_port = htons(port); */
                  /*                       sockaddr_in6->sin6_family = family; */
                  /*                       sockaddr_in6->sin6_addr.s6_addr = htonl(ip_addr); */
                  /*                       break; */

                  /*                    } */
    default: {
                 errno = EAFNOSUPPORT;
                 err_sys("SetSocketAddr error");
             }

    }
}
int ExamError(int status, const char* msg, int errnum, ...){
    int i;
    if(status < 0){
        int ok = 0;
        int t;
        va_list vl;
        va_start(vl, errnum);
        for(i = 0; i < errnum; ++i){
            t = (va_arg(vl , int));
            if(errno == t){
                ok = 1;
                break;
            }
        }

        va_end(vl);
        if(!ok) err_sys(msg);
    }
    return status;
}
