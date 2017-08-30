#include "client_socket.h"

client_socket* buildclientstruct(void)
{
    client_socket *sock;
    sock = (client_socket*)calloc(1, sizeof(client_socket));
    return sock;
}

int initsocket(client_socket* sock, char* host, int port)
{
    struct hostent *server;

    sock->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock->sockfd < 0){
        sock->err.errnum = errno;
        sock->err.errmsg = strerror(errno);
        return -1;
    }

    server = gethostbyname(host);
    if(server == NULL){
        char errmsg[] = "No such host";
        sock->err.errnum = -1;
        sock->err.errmsg = errmsg; 
        return -1;
    }
    sock->serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&sock->serv_addr.sin_addr.s_addr, server->h_length);
    sock->serv_addr.sin_port = htons(port);
    return 0;
}

int clientconnect(client_socket* sock)
{
    int ret = 0;
    ret = connect(sock->sockfd, (struct sockaddr*)&sock->serv_addr, sizeof(sock->serv_addr));
    if(ret < 0){
        sock->err.errmsg = strerror(errno);
        sock->err.errnum = errno;
        return -1;
    }

    return 0;
}

int clientwrite(client_socket* sock, char* buffer, size_t buffersize){
    int n;
    databuff *data = (databuff*)calloc(1, sizeof(databuff) + buffersize);
    if(data == NULL){
        return -1; 
    }
    memcpy(&data->buff, buffer, buffersize);
    data->totalsize = buffersize + sizeof(databuff);
    n = write(sock->sockfd, data, data->totalsize);
    if(n < 0){
        close(sock->sockfd);
        sock->err.errnum = errno;
        sock->err.errmsg = strerror(errno);
    }
    free(data);
    return n;
}

databuff* clientread(client_socket* sock){
    size_t readsize;
    size_t totalsize;
    char tempbuff[READBUFFSIZE];
    databuff *data = NULL;  

    do{
        readsize = read(sock->sockfd, tempbuff, READBUFFSIZE);
        if(data == NULL){
            totalsize = readsize;
            data = (databuff*)calloc(1, readsize);
            if(data == NULL){
                char errmsg[] = "Failed to calloc membuff";
                close(sock->sockfd);
                sock->err.errnum = -1;
                sock->err.errmsg = errmsg;
                break;
            }
            memcpy(data, &tempbuff, readsize);
        }
        else{
            data = (databuff*)realloc(data, totalsize+readsize);
            void *p = (data + totalsize/sizeof(size_t)); //really not the best way to get this offset
            memcpy(p, &tempbuff, readsize);
            totalsize += readsize;
        }
        
    }while(totalsize < data->totalsize);

    return data;
}


