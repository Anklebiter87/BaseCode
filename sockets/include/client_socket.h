#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define READBUFFSIZE 4096

typedef struct{
        int errnum;
            char *errmsg;
} errstruct;

typedef struct {
        size_t totalsize;
            char buff[];
}databuff;

typedef struct {
        int sockfd;
            struct sockaddr_in serv_addr;
                errstruct err;
}client_socket;

client_socket* buildclientstruct(void); //build the calloc memory space and returns the struct pointer
int initsocket(client_socket*, char*, int); //sets up the socket. args(client_socket struct, hostname/ip, port)
int clientconnect(client_socket*); //connects to hostname/ip
int clientwrite(client_socket*, char*, size_t); //handles writing to the socket args(client_socket struct, buffer, buffer size)
databuff* clientread(client_socket*); //reads all the data sent from the server

#endif 
