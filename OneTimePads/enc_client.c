

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>




void error(const char* message)
{
    perror(message);                                                              // Error function used for reporting issues
    exit(1);
}
void setupAddressStruct(struct sockaddr_in* address,
    int portNumber,
    char* hostname) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*)&address->sin_addr.s_addr,
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}
/* This function finds the size of files that are passed in as a perameter
*  source: https://www.geeksforgeeks.org/c-program-find-size-file/
*/

long int findSize(char file_name[])
{
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r");

    // checking if the file exist or not 
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file 
    long int res = ftell(fp);

    // closing the file 
    fclose(fp);

    return res;
}
void writeerror(int charwritten) {
    if (charwritten < 0) {
        error("CLIENT: ERROR writing to socket");
    }
}
void readerror(int charread) {
    if (charread < 0) {
        error("CLIENT: ERROR reading to socket");
    }
}
int main(int argc, char* argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;

   
    char messageLengthC[100000];
    char keyLengthC[100000];
    char buffer[100000];
    char messagebuffer[100000];
    char keybuffer[100000];

    long int messageLength;
    long int keyLength;
    FILE* openpt; //for opening plaintet
    FILE* openKey; // for opening keyfile 
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    if (argc < 4)                                                                 // Check usage & args
    {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }


    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");
    socketFD = socket(AF_INET, SOCK_STREAM, 0);                                   // Set up the socket and create the socket

    if (socketFD < 0)
    {
        error("CLIENT: ERROR opening socket");
    }

    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect to server and connect socket to address
    {
        error("CLIENT: ERROR connecting");
    }

    openpt = fopen(argv[1], "r");                                               // Open the file provided from the client
    fgets(messagebuffer, 100000, openpt);                                             // Puts the file into the message variable
    openKey = fopen(argv[2], "r");                                                // Open the key provided from the client
    fgets(keybuffer, 100000, openKey);                                                  // Puts the key into the key variable   
    long int ptlen = findSize(argv[1]);
    long int keylen = findSize(argv[2]);
    //compares the keylen to pt len: error if true
    if (keylen < ptlen) {
        fprintf(stderr, "Key \"%s\" is too short\n", argv[2]);
        return 1;

    }

    sprintf(messageLengthC, "%d", ptlen);                                 // Convert the message length into a character
    sprintf(keyLengthC, "%d", keylen);                                         // Convert the key length into a character

    //the following sends the length of the msg to the server 

    charsWritten = send(socketFD, messageLengthC, strlen(messageLengthC), 0);     // Send message to server and write to the server

    writeerror(charsWritten);
    if (charsWritten < strlen(messageLengthC))                                     // Checks to make sure all the data has transferred across socket
    {
        printf("CLIENT: data left in buffer\n");
    }

    memset(buffer, '\0', 100000);                                                 // reset buffer
    charsRead = recv(socketFD, buffer, 100000, 0);                                // recv data from socket 

    if (buffer[0] != 'h')                                                          // checks to see if we recevied hello client from the server
    {                                                                              //if we didnt the connection isnt valid
        fprintf(stderr, "ERROR Not A Valid Connection");
        exit(2);
    }



    //the following sends the keylen to the server

    charsWritten = send(socketFD, keyLengthC, strlen(keyLengthC), 0);             // Send the info to the server

    writeerror(charsWritten);
    if (charsWritten < strlen(keyLengthC))                                         // buffer is empty
    {
        fprintf(stderr, "CLIENT:data still in buffer\n");
    }

    memset(buffer, '\0', 100000);                                                 // reset buffer
    charsRead = recv(socketFD, buffer, 100000, 0);                                // recv data from socket 

    readerror(charsRead);

    //sends msg

    charsWritten = send(socketFD, messagebuffer, strlen(messagebuffer), 0);                   // send msg to server

    writeerror(charsWritten);
    if (charsWritten < strlen(messagebuffer))                                            // makes sure there is nothing in the buffer
    {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 100000);
    charsRead = recv(socketFD, buffer, 100000, 0);

    readerror(charsRead);

    //sends key 

    charsWritten = send(socketFD, keybuffer, strlen(keybuffer), 0);                           // Send message to server and write to the server

    writeerror(charsWritten);
    if (charsWritten < strlen(keybuffer))                                                // makes sure buffer is empty
    {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 100000);                                                 // reset buffer
    charsRead = recv(socketFD, buffer, 100000, 0);                                // read in data

    readerror(charsRead);

    //send encryption

    memset(buffer, '\0', 100000);                                                 // reset buffer
    charsRead = recv(socketFD, buffer, 100000, 0);                                  // read in data
    readerror(charsRead);                                                        //check for read error 

    printf("%s", buffer);                                                         // prints buffer to std out
    messageLength -= strlen(buffer);                                              // dec by the amount of chars in buffer

    while (messageLength != 0)                                                     // this ensures all the data was read in and priinted 
    {
        if (strlen(buffer) == 0)                                                     // if buffer is empty then we are done 
        {
            break;
        }
        else
        {
            memset(buffer, '\0', 100000);                                             // reset buffer everytime we get data
            charsRead = recv(socketFD, buffer, 100000, 0);                            // read possible data

            readerror(charsRead);                                                      //check for read error
            messageLength -= strlen(buffer);                                          // dec by the amount of chars in buffer
            printf("%s", buffer);                                                     //print whatever is in the buffer 
        }
    }

    close(socketFD);                                                              // Close the socket, we are done 

    return 0;
}


