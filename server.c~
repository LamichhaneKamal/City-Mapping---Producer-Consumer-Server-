#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3490
#define BACKLOG 10

int main ()
{
    //get the arguments from the ./server as number of producer and consumer
    struct sockaddr_in server;
    struct sockaddr_in dest;
    int status,socket_fd,client_fd,num;
    socklen_t size;
    char buffer[10241];
    char *buff;
    int yes = 1;
    if ((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        fprintf(stderr, "socket failure\n");
        exit(1);
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    memset(&server, 0, sizeof(server));
    memset(&dest, 0, sizeof(dest));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    if ((bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr))) == -1) {
        fprintf(stderr, "Binding Failure\n");
        exit(1);
    }
    if ((listen(socket_fd, BACKLOG)) == -1) {
        fprintf(stderr, "Listening Failure\n");
        exit(1);
    }
    while(1) {
        size = sizeof(struct sockaddr_in);
        if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size)) == -1) {
            perror("accept");
            exit(1);
        }
        printf("Server got connected %s\n", inet_ntoa(dest.sin_addr));
        while(1) {
            size = sizeof(struct sockaddr_in);
            if((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size)) == -1) {
                perror("Accept");
                exit(1);
            }
            printf("Server got connected from client %s\n",inet_ntoa(dest.sin_addr));
            while(1) {
                if((num = recv(client_fd,buffer,1024,0)) == -1) {
                    perror("recv\n");
                    exit(1);
                }
                else if (num == 0) {
                    printf("Connection closed\n");
                    break;
                }
                buffer[num] = '\0';

char command[1024];
FILE * fp;
fp = popen("./map_api", "r");
  if (fp == NULL) {
    printf("Failed to run map command\n" );
    exit(1);
  }
   while (fgets(command, sizeof(command)-1, fp) != NULL) {
    printf("%s", command);
  }
  pclose(fp);
//snprintf(command, sizeof(command), "./map_api");//,some_username, some_password); 
// net use x: \\\\server1\\shares /user:%s %s",
// FILE *in1 = popen(command, "r");        
//system(command);
strcpy(buffer, command);
                printf("Server :Msg Received %s\n", buffer );
                // );
                if ((send(client_fd, buffer, strlen(buffer),0)) == -1) {
                    fprintf(stderr, "Failure sending Message\n");
                    close(client_fd);
                    break;
                }
                printf("%Server:Msg being sent : %s \n Number of bytes sent : %d", buffer, strlen(buffer));
            }
            close(client_fd);

        }

    }
    close(socket_fd);
    return 0 ;
}
