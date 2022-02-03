#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char *argv[]) {
  // no length argument is specfied
  if (argc < 2)
    {
        printf("You must provide the length of the key\n");
        printf("Example usage: ./keygen 22\n");
        return EXIT_FAILURE;
    }
  char *length = argv[1];
  int len;
  // converts length to and int and stores as len
  sscanf(length, "%d", &len);
  char key[len + 1];
  // characters key can be composed of
  char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  srand(time(0));
  int i;
  // loops len number of times
  for (i = 0; i < len; i++) {
    // random number 0-27 
    int random = rand() % 27;
    // stores alpha letter that's in the index provided by random in key
    key[i] = alpha[random];
  }
  // null byte 
  key[i] = '\0';
  printf("%s\n", key);
  return EXIT_SUCCESS;
}