/*

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#include "helper.c"
#include <string.h>

#include <stdlib.h>
#include <stdio.h>


/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)


void error(char *msg);
void handleCurrentConnection(int);
void findSubstring(char[], char[], int);
void findFileSubstring(char[], char[], int);
void capitalize(char *word);


int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    int       clilen, pid;
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr, cli_addr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */


    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
	port = strtol(argv[1], &endptr, 0);
	if ( *endptr ) {
	    fprintf(stderr, "ECHOSERV: Invalid port number.\n");
	    exit(EXIT_FAILURE);
	}
    }
    else if ( argc < 2 ) {
	port = ECHO_PORT;
    }
    else {
	fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
	exit(EXIT_FAILURE);
    }

	
    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling bind()\n");
	exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling listen()\n");
	exit(EXIT_FAILURE);
    }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */
    clilen = sizeof(cli_addr);
    while (1) {

	/*  Wait for a connection, then accept() it  */

	if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling accept()\n");
	    exit(EXIT_FAILURE);
	}
	/*  Retrieve an input line from the connected socket
	    then simply write it back to the same socket.     */

	// Readline(conn_s, buffer, MAX_LINE-1);
	// Writeline(conn_s, buffer, strlen(buffer));
    
    handleCurrentConnection(conn_s);
    
	/*  Close the connected socket  */

	if ( close(conn_s) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling close()\n");
	    exit(EXIT_FAILURE);
	}
    }
}

void handleCurrentConnection(int sockfd) {

    int n;
    char buffer[MAX_LINE];
    char new_buff[MAX_LINE-1];
    char send_buff[MAX_LINE-1];
    FILE *fptr;
    // char send_buff2[MAX_LINE];

    Readline(sockfd, buffer, MAX_LINE-1);
    // n = read(sockfd,buffer,MAX_LINE-1);
    // if (n < 0) error("ERROR reading from socket");

    if ((buffer[0] == 'C') && (buffer[1] == 'A') && (buffer[2] == 'P')) {

        findSubstring(buffer, buffer, 6);

        capitalize(buffer);

        int length = strlen(buffer);

        sprintf(send_buff, "%d", length);
        strcat(send_buff, "\n");
        strcat(send_buff, buffer);
        //strcpy(new_buff, num);

    } else if (buffer[0] == 'F') {
        
        findFileSubstring(buffer, buffer, 7);

        if ( access(buffer, F_OK) != -1) {    // Checks if the file is present 
            fptr = fopen(buffer, "r");
            while(!feof(fptr)) {
                fgets(new_buff, MAX_LINE-1, fptr);
                strcat(send_buff, new_buff);
            }

        } else {
            strcpy(send_buff, "9\nNOT FOUND");
        }

    }

    printf("%s\n", send_buff);

    n = write(sockfd, send_buff, strlen(send_buff));

    if (n < 0) error("ERROR writing to socket");
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void capitalize(char buff[]) {
    int i = 0;
    while (i<strlen(buff)) {
        buff[i] = toupper(buff[i]);
        i++;
    }
}

void findSubstring(char g[], char p[], int diff) {

    int length = strlen(p)-8;
    int i;
    for (i=0; i<length; i++) {
        if (g[i] == '\n'){
            g[i] = '\0';
        }

        g[i] = p[i+diff-1];
    }
    g[i] = '\0';
}

void findFileSubstring(char g[], char p[], int diff) {

    int length = strlen(p)-8;
    int i;
    for (i=0; i<length-1; i++) {
        if (g[i] == '\n'){
            g[i] = '\0';
        }

        g[i] = p[i+diff-1];
    }
    g[i] = '\0';
}