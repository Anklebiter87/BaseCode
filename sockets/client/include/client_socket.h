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
/* This struct handles the errors that can be thrown while inside of the this object file */


typedef struct {
    size_t totalsize; //size 8
    char buff[];    // as big as you need it to be.
}databuff;
/*  This struct is used for coming and going data from the socket.  It was how the server or
    client gets told how big the data coming at it is. */


typedef struct {
    int sockfd;
    struct sockaddr_in serv_addr;
    errstruct err;
}client_socket;
/*  An easier way of passing the socket between function.  If an error happens errstuct will be populated and
    any connected sockets will be closed.*/


client_socket* buildclientstruct(void);
/*  Not really necessary this is just a code refactor thing.
    I got tired of calling calloc/checking
    to make sure the pointer got set.
    params:
        void
    return:
        a client_socket pointer on error NULL will be returned. client_socket was built with calloc and will have
        to have free called.
*/


int initsocket(client_socket*, char*, int);
/*  Used to setup the socket and assign it to the client_socket
    also does the gethostinfo, and setup the serv_addr struct
    prams:
        client_socket*
        char* hostname or ip
        int port_number
    return:
        will return 0 if everything is configured correctly or -1 on error.
        Check errstruct for what actually happened*/


int clientconnect(client_socket*);
/*  Connects to the server passed to initsocket or handles the setting the errors on failure
    params:
        client_socket*
    return:
        will return 0 if a connection get established correctly or -1 on error.
        Check errstruct for what actually happened. */


int clientwrite(client_socket*, char*, size_t);
/*  My implementation of how to send all the data to the server. Takes a buffer and builds a datastruct.
    see the datastruct for how the server will receive the data
    params:
        client_socket*
        char* buffer
        size_t buffer_size
    returns:
        Will return the amount of data writen to the socket. In the event of an error -1 will be returned
        the connection will be closed and errstruct will be populated.*/


databuff* clientread(client_socket*);
/*  My implementation of how to read all the data from the server.
    This populates a datastruct and returns all the data.
    Its a bad implementation for huge files (IE: >500mb) but it will use all
    your memory and not care.
    params:
        client_socket*
    return:
        Will return a databuff struct containing the size of the data and the data. It was created with calloc and
        will have to have free called.  On error a few things could happen. If an error happened on realloc,
        databuff will be partially populated with whatever data was recived and passed back. If the error is on
        calloc NULL will be returned. In both cases the connection will be closed and errostuct will be populated.
        */
#endif 
