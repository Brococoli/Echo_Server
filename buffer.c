#include "buffer.h"

int isBufFull(NormalBuffer* buf){
    if(buf == NULL) return -1;
    return buf->front == buf->size - 1;
    //because the last position is '\0';
}
int isBufEmpty(NormalBuffer* buf){
    if(buf == NULL) return -1;
    return buf->front == buf->rear;
}

NormalBuffer* InitBuf(int size){
    if(size < 0 || size > MAXBUFSIZE) {
        fprintf(stderr, "size is larger than %d", MAXBUFSIZE);
        return NULL;
    }
    NormalBuffer* buf = (NormalBuffer*)malloc(sizeof(NormalBuffer));
    buf->size = size;
    buf->ptr = (char*)malloc(sizeof(char)*size);
    if(buf->ptr == NULL) {
        err_sys("InitBuf error: malloc error");
        return NULL;
    }
    buf->front = buf->rear = 0;
    return buf;
}
void FreeBuf(NormalBuffer* buf){
    if(buf == NULL) return ;
    if(buf->ptr) free(buf->ptr);
    buf->front = buf->rear = 0;
}

int WriteToFile(int fd, NormalBuffer* buf){
    // same as the read, The function return the byte number which actually read from fd.
    if(buf == NULL) return -1;
    if(isBufEmpty(buf)) return 0;
    int status = write(fd, buf->ptr + buf->rear, buf->front - buf->rear);
    if(status > 0) buf->rear += status;
    if(buf->rear != 0 && buf->rear == buf->front)
        buf->rear = buf->front = 0;
    return status;
}
int ReadFromFile(int fd, NormalBuffer* buf){
    // same as the write, The function return the byte number which actually write to file.
    if(buf == NULL) return -1;
    if(isBufFull(buf)) return 0;
    int status = read(fd, buf->ptr + buf->front, buf->size - buf->front - 1);
    if(status > 0) buf->front += status; //in case of the status is -1;
    return status;
}
