#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char*argv[])
{
    struct sockaddr_storage connectinfo;
    socklen_t connectinfosize;
    struct addrinfo hints, *ipinfo;
    int sockfd;
    int newsockfd;
    int status;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // removes need for hard-coding server IP address
    hints.ai_flags = AI_PASSIVE;


    // sets server port to 3892
    // NULL IP address filled by getaddrinfo due to AI_PASSIVE flag
    status = getaddrinfo(NULL, "3892", &hints, &ipinfo);


    if(status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }


    // Initializes the socket descriptor, binds the port, then listens
    sockfd = socket(ipinfo->ai_family, ipinfo->ai_socktype, ipinfo->ai_protocol);
    bind(sockfd, ipinfo->ai_addr,ipinfo->ai_addrlen);
    listen(sockfd, 10);
    connectinfosize = sizeof connectinfo;


   // loop to keep looking for new connections
    for(;;) {
        // accepts connection on newsockfd; allows listen persistence of sockfd
        newsockfd = accept(sockfd, (struct sockaddr *)&connectinfo, &connectinfosize);
        int pid;
        if( (pid=fork())== -1){
            close(newsockfd);
            continue;
        }
        else {
            char recv_buf[100];
            char send_buf[200];
            bzero(recv_buf, 100);
            bzero(send_buf, 200);
            recv(newsockfd, recv_buf, sizeof recv_buf, 0);          
            printf("You received: %s\n", recv_buf);
            strncpy(send_buf, "You sent this to the server: ", 29);
            strncat(send_buf, recv_buf, sizeof recv_buf);
            send(newsockfd, send_buf, sizeof send_buf, 0);
            printf("You sent: %s\n", send_buf);
            close(newsockfd);
        }
    }


    // closes the listener
    close(sockfd);
    freeaddrinfo(ipinfo);
    return 0;
}