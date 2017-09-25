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
#include <errno.h>

#define MAXBUFSIZE 100
#define MAXMSGSIZE 2048
#define EXIT 1
#define LS 2
#define GET 3
#define PUT 4
#define DELETE 5
#define CLOSE 6

/* You will have to modify the program below */

int parse_command(const char* input) {
    //printf("|%s|\n", input);
    if (!strcmp(input, "exit")){
        return EXIT;
    }
    else if (!strcmp(input,"ls")){
        return LS;
    }
    else if (!strcmp(input, "close")){
        return CLOSE;
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

int main (int argc, char * argv[])
{

	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	char response[MAXMSGSIZE];

	struct sockaddr_in remote, from_addr;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	//if ((sock = **** CALL SOCKET() HERE TO CREATE A UDP SOCKET ****) < 0)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}
	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

	char command[MAXBUFSIZE];
	int loop = 1;

	while(loop){
        bzero(&command, sizeof(command));
		printf("Enter a command: ");

		fgets(command, MAXBUFSIZE, stdin);
		if (command[strlen(command)-1] == '\n') command[strlen(command)-1] = '\0'; //removing trailing newline

		switch(parse_command(command)){
        case GET:
            nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            // Blocks till bytes are received
            bzero(response,sizeof(response));
            nbytes = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
            printf("%s", response);
            break;

        case PUT:
            nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            // Blocks till bytes are received
            bzero(response,sizeof(response));
            nbytes = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
            printf("%s", response);
            break;

        case DELETE:
            nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }

            // Blocks till bytes are received
            bzero(response,sizeof(response));
            nbytes = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
            printf("%s", response);
            break;

        case LS:
            nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }

            // Blocks till bytes are received
            bzero(response,sizeof(response));
            nbytes = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));
            printf("%s", response);

            break;

        case EXIT:
            loop = 0;
            nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
            if ( nbytes == -1) {
                printf("Error sendto(): %s\n", strerror(errno));
            }
            break;

        case CLOSE:
            loop = 0;
            break;

        default:
            printf("invalid command. try again.\n\n");
            break;
		}
	}

	/******************
	  sendto() sends immediately.
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	/*printf("about to send.\n");
	nbytes = sendto(sock, command, strlen(command), 0, (struct sockaddr *) &remote, sizeof(remote));
	if ( nbytes == -1) {
        printf("Error sendto(): %s\n", strerror(errno));
	}
    printf("nbytes: %d\n", nbytes);


	// Blocks till bytes are received
	struct sockaddr_in from_addr;
	bzero(buffer,sizeof(buffer));
    printf("about to recv.\n");
	nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &from_addr, (unsigned int * restrict) sizeof(from_addr));

	printf("Server says %s\n", buffer);*/

	close(sock);
}

