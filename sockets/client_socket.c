#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include <stdio.h>

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
            void *p = (data + totalsize/8);
            memcpy(p, &tempbuff, readsize);
            totalsize += readsize;
        }
        
    }while(totalsize < data->totalsize);

    return data;
}


int main(int argc, char *argv[]) {
    int portno, n;
    client_socket *sock;
    databuff *data;
    FILE *f;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    sock = buildclientstruct();
    portno = atoi(argv[2]);

    /* Create a socket point and setup serv_addr struct*/
    if(initsocket(sock, argv[1], portno) < 0){
        printf("ERROR %s\n", sock->err.errmsg);
        exit(sock->err.errnum);
    }

    /* Now connect to the server */
    if(clientconnect(sock) < 0){
        printf("ERROR %s\n", sock->err.errmsg);
        exit(sock->err.errnum);
    }

    /* Now ask for a message from the user, this message
     * will be read by server
     */

/*    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

     Send message to the server 
    n = clientwrite(sock, buffer, strlen(buffer));
    if (n < 0) {
        printf("ERROR: %s\n", sock->err.errmsg);
        exit(sock->err.errnum);
    }
*/

    /* Now read server response */
    data = clientread(sock);

    if (data == NULL) {
        printf("ERROR: %s\n", sock->err.errmsg);
        exit(sock->err.errnum);
    }

    f = fopen("/tmp/output", "w");
    if(f == NULL){
        perror("Failed to make file");
    }
    
    fwrite((char *)&data->buff,data->totalsize-8,1,f);
    fclose(f);


    free(data);
    close(sock->sockfd);
    free(sock);
    return 0;
}
