#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 100
#define MAXMSGSIZE 2048
#define EXIT 1
#define LS 2
#define GET 3
#define PUT 4
#define DELETE 5

int parse_buffer(const char* input) {
    //printf("|%s|\n", input);
    if (!strcmp(input,"ls")){
        return LS;
    }
    else if (!strcmp(input, "exit")){
        return EXIT;
    }
    else {
        if (!strcmp(input, "")){
            return -1;
        }
        char temp[MAXBUFSIZE];
        strcpy(temp, input);
        const char* cmd = strtok(temp, " ");

        if (!strcmp(cmd, "get")){
            return GET;
        }
        else if (!strcmp(cmd, "put")){
            return PUT;
        }
        else if (!strcmp(cmd, "delete")){
            return DELETE;
        }
        else return -1;
    }
}

int main (int argc, char * argv[] )
{
	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote, from_addr;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	//if ((sock = **** CALL SOCKET() HERE TO CREATE UDP SOCKET ****) < 0)
    if ((sock = socket(PF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("unable to create socket\n");
	}
	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

	/******************
	  Once we've created a socket, we must bind that socket to the
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

    printf("Server Running.\n");

	int loop = 1;
	char filename[MAXBUFSIZE];
	char path[PATH_MAX];
	FILE *file;
	char fileBuffer[MAXMSGSIZE];
	char response[MAXMSGSIZE];
	while(loop){
        char msg[MAXMSGSIZE];
        bzero(msg,sizeof(msg));
        strcpy(msg, "Server Response:\n");
        //waits for an incoming message
        bzero(buffer,sizeof(buffer));
        socklen_t addrlen = sizeof(remote);
        nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &addrlen);

        switch(parse_buffer(buffer)){
        case GET:
            bzero(filename,sizeof(filename));
            memcpy(filename, &buffer[4], MAXBUFSIZE-4);

            if (file = fopen(filename, "rb")){
                fseek(file, 0, SEEK_END);
                size_t fileSize = ftell(file);
                fseek(file, 0, SEEK_SET);

                int numBuffers = ceil((double)fileSize / (MAXMSGSIZE-10));
                int currentBuffer = 1;
                bzero(msg,sizeof(msg));
                sprintf(msg, "%d", numBuffers);
                nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
                if ( nbytes == -1) {
                    printf("Error sendto(): %s\n", strerror(errno));
                }

                while(currentBuffer <= numBuffers){
                    //printf("in while.\n");
                    bzero(msg,sizeof(msg));

                    /*fread(fileBuffer, MAXMSGSIZE-10, 1, file);

                    char sequence[10];
                    sprintf(sequence, "%04d|%04d|", currentBuffer, numBuffers);
                    strcpy(msg, sequence);
                    strcat(msg, fileBuffer);*/
                    fread(msg, MAXMSGSIZE, 1, file);

                    //printf("|%s|\n", msg);
                    nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
                    if ( nbytes == -1) {
                        printf("Error sendto(): %s\n", strerror(errno));
                    }

                    //printf("\n\n\nNEW BUFFER\n|%s|\n", msg);
                    currentBuffer += 1;
                }

                fclose(file);
                bzero(msg,sizeof(msg));
                strcpy(msg, "Server Response:\n");
                strcat(msg, "Got ");
                strcat(msg, filename);
                strcat(msg, "\n");
                nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
                if ( nbytes == -1) {
                    printf("Error sendto(): %s\n", strerror(errno));
                }

            } else {
                strcat(msg, "File does not exist.\n");
                nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
                if ( nbytes == -1) {
                    printf("Error sendto(): %s\n", strerror(errno));
                }
            }

            printf("GET request completed.\n");
            break;

        case PUT:
            bzero(filename,sizeof(filename));
            memcpy(filename, &buffer[4], MAXBUFSIZE-4);
            bzero(response,sizeof(response));
            nbytes = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
            int packets = atoi(response);
            int received = 0;

            FILE *file = fopen(filename, "w");
            fseek(file, 0, SEEK_SET);
            for (int i = 1; i <= packets; i++){
                recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
                received += 1;

                fwrite(response, strlen(response), 1, file);
            }
            fclose(file);
            bzero(msg,sizeof(msg));
            strcpy(msg, "Server Response:\n");
            strcat(msg, "Put ");
            strcat(msg, filename);
            strcat(msg, "\n");
            nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            printf("PUT request completed.\n");
            break;

        case DELETE:
            bzero(filename,sizeof(filename));
            memcpy(filename, &buffer[7], MAXBUFSIZE-7);

            if (file = fopen(filename, "r")){
                fclose(file);
                char removeCommand[MAXBUFSIZE];
                strcpy(removeCommand, "/bin/rm ");
                strcat(removeCommand, filename);

                FILE *rm = popen(removeCommand, "r");
                pclose(rm);

                strcat(msg, "Deleted ");
                strcat(msg, filename);
                strcat(msg, "\n");
            } else {
                strcat(msg, "File does not exist.\n");
            }

            nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            printf("DELETE request completed.\n");
            break;

        case LS: ;
            FILE *ls = popen("/bin/ls -la", "r");
            if (ls != NULL) {
                while (fgets(path, PATH_MAX, ls) != NULL) {
                    strcat(msg, path);
                }
                pclose(ls);
            } else {
                strcat(msg, "Unable to list directory.\n");
            }

            nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            printf("LS request completed.\n");
            break;

        case EXIT:
            printf("EXIT command received from client\n");
            loop = 0;
            break;

        default:
            strcat(msg, buffer);
            strcat(msg, " was not understood.\n");
            nbytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            break;
		}
	}

	close(sock);
	printf("Server Closed.\n");
}

