/*

  ECHOCLNT.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo client.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  Our own helper functions  */
#include "helper.c"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);

void error(char *msg);


FILE *write_ptr;
/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
    printf("ECHOCLNT: Invalid port supplied.\n");
    exit(EXIT_FAILURE);
    }
    

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
    exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
    printf("ECHOCLNT: Invalid remote IP address.\n");
    exit(EXIT_FAILURE);
    }

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
        printf("ECHOCLNT: Error calling connect()\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection started.\n");
    }


    /*  Get string to echo from user  */

    // printf("Enter the string to echo: ");
    // fgets(buffer, MAX_LINE, stdin);
    

    /*  Send string to echo server, and retrieve response  */

    // Writeline(conn_s, buffer, strlen(buffer));
    // Readline(conn_s, buffer, MAX_LINE-1);

    char c, d, *temp;     // Initial variables
    char inp[1];
    char front[MAX_LINE];
    char back[MAX_LINE-1];
    char filename[MAX_LINE-1];
    temp = buffer;

    char new_buffer[MAX_LINE];
    int sockfd = conn_s;  // saving conn_s in sockdf
   
    size_t      n, ns, nleft;
    ssize_t     nwritten;

    while (1) {

        printf("Please enter 's' to input a sting, 't' to input a file and 'q' to quit: ");  // Promting for users direction
        
        int j = 0;
        while ((c = getchar()) != '\n')         // Read the string seperated by new line
            inp[j++] = c;

        if (*inp == 's') {                      // If 's': string case

            int i = 0;
            printf("Please enter a string to continue: ");  // Prompt for the string
            while ((c = getchar()) != '\n')                 // Getting string input until new line
                buffer[i++] = c;

            strcpy(front, "CAP\\n");                        // String concatenation to create CAP\nxxxx\n
            strcpy(back, buffer);
            strcat(back, "\\n");
            strcat(front, back);
            front[strlen(front)] = '\n';                    // Adding new line at the end of the string 

        } else if (*inp == 't') {                           // File case

            int i = 0;
            printf("Please enter the file name to send: ");  // Prompt for the file name
            while ((c = getchar()) != '\n')                  // Getting string input until new line
                buffer[i++] = c;
            strcpy(filename, buffer);
            strcpy(front, "FILE\\n");                        // Concatenating string to send FILE\nxxxx\n
            strcpy(back, buffer);
            strcat(back, "\\n");
            strcat(front, back);
            front[strlen(front)] = '\n';                     // Adding new line at the end of the string
        
        } else if (*inp == 'q') {
            printf("session closed.");                       // Close if user enter q in initial prompt
            break;
        } else {
            printf("close");                                 // close by default
            break;
        }
        // printf("%s\n", front);

        n = write(sockfd,front,strlen(front));              // Writing the string stream to the socket
        if (n < 0) 
             error("ERROR writing to socket");
        
        read(sockfd, buffer, MAX_LINE-1);                   // Reading back from the socket into buffer

        char size_buff[10];
        int k = 0;
        while ((d = buffer[k]) != '\n')                     // Declaring temporary variable to calculate size
            size_buff[k++] = d;
    
        
        printf("%s\n",buffer);                              // Printing the buffer read from the socket     


        int size_of_file = atoi(size_buff);                 // Temporary variable that has size parameters


        if (*inp == 't') {                                  // For file case
            // buffer = strtok(buffer, "\n");
            write_ptr = fopen(filename, "wb");              // operning the file pointer for binary write
            if (write_ptr) 
                fwrite(buffer, size_of_file, 1, write_ptr); // Write buffer to the file until reached at end
        }


        if (ns < 0) 
             error("ERROR reading from socket");

       
        // printf("%s\n", size_buff);
        break;
    }

    if ( close(conn_s) < 0 ) {                              // Closing connection after all the tasks
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection closed.\n");
    }
    

    /*  Output echoed string  */

    // printf("Echo response: %s\n", buffer);

    return EXIT_SUCCESS;
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {

    *szAddress = argv[1];                                   // Setting defauly port and address for the client

    *szPort = argv[2];

 //    int n = 1;

 //    while ( n < argc ) {
    // if ( !strncmp(argv[n], "-a", 2) || !strncmp(argv[n], "-A", 2) ) {
        
    // }
    // else if ( !strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2) ) {
        
    // }
    // else if ( !strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2) ) {
    //     printf("Usage:\n\n");
    //     printf("    timeclnt -a (remote IP) -p (remote port)\n\n");
    //     exit(EXIT_SUCCESS);
    // }
    // ++n;
 //    }

    return 0;
}

void error(char *msg)                                       // Error msg function to print errors            
{
    perror(msg);
    exit(0);
}
