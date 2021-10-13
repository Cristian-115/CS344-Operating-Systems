
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>



//most of the following code that deals with sockets, adresses, and connections is from the explorations and the replits
void error(const char* message)
{
    perror(message);                                                              // Error function used for reporting issues
    exit(1);
}
void setupAddressStruct(struct sockaddr_in* address,
    int portNumber) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

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



char* decrypt(char* cipher, char* key) { //encrypt a given message
    int i;
    char n;
    char c;

    //for the length of the message
    for (i = 0; i < strlen(cipher); i++)
    {
        // the following decrypts chars as stated on the assignment page
        if (cipher[i] == '\n') {
            break;
        }

       
        if (cipher[i] == ' ') {
            n = 26;
        }
        else {

            n = cipher[i] - 65;

        }
        if (key[i] == ' ') {
            c = 26;
        }

        else {

            c = key[i] - 65;

        }
        n = (n - c);
        if (n < 0) {
            n += 27;
        }
        //add them and mod 27 to get the encrypted character.

        if (n == 26) {
            cipher[i] = ' ';
        }
        //  integer plus ascii of A
        else
        {
            cipher[i] = n + 65;
        }


    }
    strcat(cipher, "\n");
    cipher[i + 1] = '\0';

    return cipher;
}
void readerror(int charread) {
    if (charread < 0) {
        error("CLIENT: ERROR reading to socket");
    }
}
int main(int argc, char const* argv[])
{
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead, pid, messageLength, keyLength, i, status;

    socklen_t sizeOfClientInfo;
    char buffer[100000];
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    if (argc < 2)                                                                 // Check usage & args
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }
    setupAddressStruct(&serverAddress, atoi(argv[1]));
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);                             //set up socket

    if (listenSocketFD < 0)
    {
        error("ERROR opening socket");
    }

    if (bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)  // Enable the socket to begin listening and connect socket
    {
        error("ERROR  binding");
    }
    listen(listenSocketFD, 5);                                                    // Fopen the socket for listening

    while (1)
    {
        char temp[100000];
        char key[100000];

        sizeOfClientInfo = sizeof(clientAddress);
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr*)&clientAddress, &sizeOfClientInfo);

        if (establishedConnectionFD < 0)                                             // Make sure that the connection has been established
        {
            error("ERROR on accept");
        }

        pid = fork();

        switch (pid) {
        case -1: { //handles forking error
            fprintf(stderr, "error forking\n");
            exit(1);
        }
        case 0: {
            /* gets the length of plaintext */
            memset(buffer, '\0', 100000);                                             // sets up the buffer
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);             // read in the messege to buffer

            if (charsRead < 0)                                                         // checks for a bad return from recv
            {
                error("ERROR reading from socket");
            }

            messageLength = atoi(buffer);                                              //get the length of plaintext
            char* memo = "decrypt";                                               //let server know we got the plaintext
            charsRead = send(establishedConnectionFD, memo, sizeof(memo), 0);


            /* gets the length of key */
            memset(buffer, '\0', 100000);                                             // reset buffer
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);     // Read the client's key from the socket

            keyLength = atoi(buffer);                                                 //get the keylen
            // Send a Success message back to the client
            char* servermemo = "Server: I recieved the key length";                   //let server know we got the key
            charsRead = send(establishedConnectionFD, servermemo, sizeof(servermemo), 0);





            /*reads in the plaintext */
            memset(buffer, '\0', 100000);                                             // reset buffer
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);     // Read the plaintext

            readerror(charsRead);

            strcat(temp, buffer);                                                  // the data from out buffer to the temp buffer
            messageLength -= strlen(buffer);                                       //decrease by the amount we read in

            while (messageLength != 0)                                                 // if msglen is 0 then we read in all the data
            {

                memset(buffer, '\0', 100000);                                         // reset buffer for every iteration
                charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Read data 

                readerror(charsRead);
                messageLength -= strlen(buffer);                                      //decrease by the amount we read in
                strcat(temp, buffer);                                              // add data from buffer into temp

            }
            // let server know we were successful 
            char* servermemo2 = "Server: Success";
            charsRead = send(establishedConnectionFD, servermemo2, sizeof(buffer), 0); // Send success back






            memset(buffer, '\0', 100000);                                             // reset buffer
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);             // read key data

            readerror(charsRead);

            strcat(key, buffer);                                                      // adds out key data to key buffer
            keyLength -= strlen(buffer);                                              //dec by the amount we read in

            while (keyLength != 0)                                                     // if keylen is 0 then we read in all the data
            {

                memset(buffer, '\0', 100000);                                         // this is similar to whats above just for the key
                charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);

                readerror(charsRead);
                keyLength -= strlen(buffer);
                strcat(key, buffer);

            }

            charsRead = send(establishedConnectionFD, servermemo2, sizeof(buffer), 0);

            readerror(charsRead);

            

            charsRead = send(establishedConnectionFD, decrypt(temp, key), strlen(temp), 0); // Send encrypted message back

            if (charsRead < strlen(temp))                                           // Checks to make sure all the data has transferred across socket
            {
                printf("WARNING: data still in socket\n");
            }


        default:
        {
            pid_t parentpid = waitpid(pid, &status, WNOHANG);
        }
        }

        }



        close(establishedConnectionFD);                                             // Close the existing socket which is connected to the client
    }
    close(listenSocketFD);                                                        // Close the listening socket

    return 0;
}

