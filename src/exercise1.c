#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <openssl/sha.h>


int my_sha(char* inp, unsigned char* outp, int size)
{
  SHA256_CTX c;
  SHA256_Init(&c);
  SHA256_Update(&c, inp, size);
  SHA256_Final(outp, &c);
  return 0;
}

int lengthof(char* a)
{
  int res = 0;
  while(a[res] != '\0')
  {
    res++;
  }
  return res;
}

int randString(char* dest, int len)
{    
  for(int i = 0; i < len; i++)
  {
    dest[i] = (char) (rand() % 26 + 'A');
  }
  return 0;
}

int findNonce(char* inp, int length, unsigned char* result, int* attempts)
{
  *attempts = 0;
  while(1)
  {
    *attempts += 1;
    randString(inp + length, 100 - length);
    my_sha(inp, result, 100);
    if( ( * (int*) result & 0x0000ffff ) == 0) break;
  }


  return 0;
}

int main(int argc, char* argv[])
{
  srand(time(NULL));
  if(argc != 2)
  {
    printf("Supply 1 string as argument\n");
    return 1;
  }

  unsigned char* result = (unsigned char*) malloc(SHA256_DIGEST_LENGTH);
  char* inp = (char*) malloc(100);
  int length = lengthof(argv[1]);

  if(length > 70)
  {
    printf("Input must be less than 71 characters\n");
  }

  strncpy(inp, argv[1], length);

  int* attempts = (int*) malloc(sizeof(int));
  findNonce(inp, length, result, attempts);
  printf("Padded string is: %s\n", inp);
  printf("Nonce is: %s\n", inp + length);
  printf("Found in %d attempts\n", *attempts);
  printf("Hash is: ");
  for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
  {
    printf("%02x", result[i]);
  }
  printf("\n");
  return 0;
}