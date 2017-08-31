#include "client_socket.h"

client_socket* buildclientstruct(void)
{
/*  Not really necessary this is just a code refactor thing.
    I got tired of calling calloc/checking
    to make sure the pointer got set.*/
    client_socket *sock;
    sock = (client_socket*)calloc(1, sizeof(client_socket));
    if(sock == NULL){
        return NULL;
    }
    return sock;
}

int initsocket(client_socket* sock, char* host, int port)
{
    /*  Used to setup the socket and assign it to the client_socket
        also does the gethostinfo, and setup the serv_addr struct*/
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
    memcpy((char *)&sock->serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    sock->serv_addr.sin_port = htons(port);
    return 0;
}

int clientconnect(client_socket* sock)
{
    /*  connects to the server or handles the setting the errors */
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
    /*  My implementation of how to send all the data to the server
        see the datastruct for how the server will receive the data */
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
/*  My implementation of how to read all the data from the server.
    This populates a datastruct and returns all the data.
    Its a bad implementation for huge files (IE: >500mb) but it will use all
    your memory and not care.*/

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
            void *tmp = (databuff*)realloc(data, totalsize+readsize);
            if(tmp == NULL){
                char errmsg[] = "Could not allocate memory";
                close(sock->sockfd);
                sock->err.errnum = -1;
                sock->err.errmsg = errmsg;
                break;
            }
            data = tmp;
            void *p = (data + totalsize/sizeof(size_t)); //really not the best way to get this offset
            memcpy(p, &tempbuff, readsize);
            totalsize += readsize;
        }
        
    }while(totalsize < data->totalsize);
    return data;
}


