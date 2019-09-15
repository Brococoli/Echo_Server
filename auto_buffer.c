#include "auto_buffer.h"


AutoBufferNode* InitBufNode(){
    AutoBufferNode* node = (AutoBufferNode*)malloc(sizeof(AutoBufferNode));
    if(node == NULL) return NULL;
    node->data_ptr = (char*)malloc(sizeof(char)*MAXBUFSIZE);
    if(node->data_ptr == NULL) return NULL;
    node->next = NULL;
    return node;
}
AutoBuffer* InitBuf(int size){
    AutoBuffer* buf = (AutoBuffer*)malloc(sizeof(AutoBuffer));
    if(buf == NULL) return NULL;
    buf->front = buf->rear = InitBufNode();
    buf->front_ptr = buf->rear_ptr = 0;
    return buf;
}
void PushBufNode(AutoBuffer* buf){
    buf->rear->next = InitBufNode();
    buf->rear = buf->rear->next;
    buf->rear_ptr = 0;
}
void FreeBufNode(AutoBufferNode* buf_node){
    if(buf_node == NULL) return;
    if(buf_node->data_ptr != NULL){
        free(buf_node->data_ptr);
    }
    free(buf_node);
}
void PopBufNode(AutoBuffer* buf){
    AutoBufferNode* temp = buf->front;
    buf->front = buf->front->next;
    buf->front_ptr = 0;
    FreeBufNode(temp);
}
void FreeBuf(AutoBuffer* buf){
    if(buf == NULL) return;
    AutoBufferNode* p, *q;
    for(p = buf->front; p!=NULL ; p = q){
        q = p->next;
        FreeBufNode(p);
    }
    free(buf);
}
int isBufFull(AutoBuffer* buf){
    if(buf == NULL) return -1;
    return 0;
}
int isBufEmpty(AutoBuffer* buf){
    if(buf->front != buf->rear) return 0;
    else if(buf->front_ptr != buf->rear_ptr) return 0;
    else return 1;
}
int ReadFromFile(int fd, AutoBuffer* buf){
    if(buf == NULL) return -1;
    int status = read(fd, buf->rear->data_ptr + buf->rear_ptr, MAXBUFSIZE - buf->rear_ptr);
    if(status > 0) buf->rear_ptr += status;
    if(buf->rear_ptr == MAXBUFSIZE)
        PushBufNode(buf);
    return status;
}
int WriteToFile(int fd, AutoBuffer* buf){
    if(buf == NULL) return -1;
    if(isBufEmpty(buf)) return 0;
    int status;
    if(buf->front == buf->rear){
        status = write(fd, buf->front->data_ptr + buf->front_ptr, buf->rear_ptr - buf->front_ptr);
        if(status > 0) buf->front_ptr += status;
        if(buf->front_ptr != 0 && buf->front_ptr == buf->rear_ptr)
            buf->front_ptr = buf->rear_ptr = 0;
    }
    else{
        status = write(fd, buf->front->data_ptr + buf->front_ptr, MAXBUFSIZE - buf->front_ptr);
        if(status > 0) buf->front_ptr += status;
        if(buf->front_ptr == MAXBUFSIZE){
            PopBufNode(buf);
        }
    }
    return status;
}
