#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, keyRead;
  char buffer[300000];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 10); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    // Get the message from the client and display it
    memset(buffer, '\0', 300000);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, sizeof(buffer), 0);
    int bufferLen;
    bufferLen = strlen(buffer) - 1;
    // prints error if the client connecting does not have the right identifier ("@")
    // wrong client connecting to server
    if((buffer[bufferLen] != '@')) {
      fprintf(stderr,"SERVER: ERROR dec_client cannot use enc_server");
      charsRead = send(connectionSocket, 
                    "\0", 400000, 0);
      close(connectionSocket);
    }
    else{
    if (charsRead < 0){
      error("ERROR reading from socket");
    }
    char *plainData;
    char *keyData;
    char *saveptr;
    // splits buffer by the new line so plain and key variables contains plaintext and key data respectively
    plainData = strtok_r(buffer, "\n", &saveptr);
    keyData = strtok_r(NULL, "\n", &saveptr);
    keyData[strlen(keyData)] = '\0';
    char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int arr[70000];  // holds integers that correspond to one-time pads math
    int k = 0;  // index for one-time pad math array
    // iterates through all character in plainData
    while(plainData[k] != '\0') {
      // loops through all 27 possible characters
      for(int i = 0; i < 28; i++) {
        // character is found in the alpha
        if (alpha[i] == plainData[k]) {
          // index is placed in one-time pad math array
          arr[k] = i;
        }
      }
      k++;
    }
    int j = 0;  // index for one-time pad math array
    // iterates through all characters in keyData
    while(keyData[j] != '@') {
      // made it to the end of one-time pad math array
      if (j == k) {
        break;
      }
      // loops through all 27 possible characters
      for(int i = 0; i < 28; i++) {
        // character is found in the alpha
        if (alpha[i] == keyData[j]) {
          arr[j] += i;  // adds index of alpha to one-time pad math array
          arr[j] = arr[j] % 27;  // remainder of arr[j] / 27 is stored in one-time pad math array
        }
      }
      j++;
    }
    int f = 0;
    char letter;
    // text that is sent back to client
    char encrypTxt[70000];
    // loops through each int in one-time pad math array
    while(1) {
      int n = arr[f];
      //alpha that corresponds to final solution of one-time pad 
      letter = alpha[n];
      // last character is null-byte
      if (f == j) {
        encrypTxt[f] = '\0';
        break;
      }
      // puts letter in encrypted text area
      encrypTxt[f] = letter;
      f++;
    }
    // Send a Success message back to the client
    charsRead = send(connectionSocket, 
                    encrypTxt, 400000, 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket); 
  }
    }
    
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
