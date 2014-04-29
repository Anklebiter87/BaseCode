#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
int main(int argc, char*argv[])
{
    struct addrinfo hints, *ipinfo;
    char* send_msg = "I connected to you";
    int len = strlen(send_msg);
    char recv_msg[100 + len];
    int sent_bytes, recv_bytes;
    int connect_fd;
    int connectedtest;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    // sets the fields in the addrinfo structs
    getaddrinfo("127.0.0.1", "3892", &hints, &ipinfo);
    connect_fd = socket(ipinfo->ai_family, ipinfo->ai_socktype, ipinfo->ai_protocol);
    if(connect_fd == -1){
        printf("socket error\n");
        return(1);
    }


    // Attempt to connect to server
    connectedtest = connect(connect_fd, ipinfo->ai_addr, ipinfo->ai_addrlen);
    if (connectedtest == -1){
        printf("Failed to connect\n");
        return(2);
    }


    // Connection successful; attempt to send & receive data
    len = strlen(send_msg);
    sent_bytes = send(connect_fd, send_msg, len, 0);
    printf("You sent: %s\n", send_msg);
    bzero(recv_msg, 100 + len);
    recv_bytes = recv(connect_fd, recv_msg, sizeof recv_msg, 0);
    printf("You received: %s\n", recv_msg);


    // Close connection
    close(connect_fd);


    printf("You sent %d bytes\n", sent_bytes);
    printf("You received %d bytes:\n", recv_bytes);
    return 0;
}