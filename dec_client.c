#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <ctype.h>


/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Send ciphertext file and key file to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

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
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, keyWritten, charsRead, cipherTextCount, keyCount;
  char ch;
  struct sockaddr_in serverAddress;
  char buffer[150000];
  FILE* fp;
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"Not enough arguments given\n"); 
    exit(0); 
  } 
  // open ciphertext file
  fp = fopen(argv[1], "r");
  char *cipherTextData = NULL;
  size_t len = 0;
  // data from ciphertext is saved in cipherTextData
  getline(&cipherTextData, &len, fp);
  int i = 0;
  // iterates through each character in cipherTextData
  while(cipherTextData[i] != '\0') {
    cipherTextCount++;
    // character is not part of the alphabet
    if(!isalpha(cipherTextData[i])) {
      // if the character is not a space or a new line character the program prints an erro
      if ((cipherTextData[i] != ' ') && (cipherTextData[i] != '\n'))  {
        fprintf(stderr, "CLIENT: ERROR, %s contains invalid characters\n", argv[1]);
        exit(1);
      }
    }
    i++;
  }
  fclose(fp);  // close ciphertext file
  // opens  key file
  fp = fopen(argv[2], "r");
  char *keyData = NULL;
  size_t lenKey = 0;
  // data from key is saved in keyData
  getline(&keyData, &lenKey, fp);
  i = 0;
  // iterates through each character in key file
  while(keyData[i] != '\0') {
    keyCount++;  // increase character count for key file
    i++;
  }
  // prints error if key file is shorter than plaintext file
  if (keyCount < cipherTextCount) {
    fprintf(stderr, "CLIENT: ERROR, key file is shorter than plain text file\n");
    exit(1);
  }
  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }
  // puts cipher and key data together  
  char *data = malloc(strlen(cipherTextData)+ strlen(keyData) + 1);
  strcpy(data, cipherTextData);
  strcat(data, keyData);
  // Send message to server
  // Write to the server
  charsWritten = send(socketFD, data, strlen(data), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  // prints encrpypted plaintext file
  printf("%s\n", buffer);
  memset(buffer, '\0', sizeof(buffer));
  // Close the socket
  close(socketFD); 
  return 0;
}