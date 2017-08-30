

#include <stdio.h>
#include "client_socket.h"


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
