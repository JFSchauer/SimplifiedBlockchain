#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Block
{
  int miner;
  char* nonce;
  unsigned char* hash;
  struct Block* next;
} my_block;

// my_sha uses openssl library to compute outp, the SHA256 hash of inp
// inp must be size bytes, outp must be 32 bytes.
int my_sha(char* inp, unsigned char* outp, int size)
{
  SHA256_CTX c;
  SHA256_Init(&c);
  SHA256_Update(&c, inp, size);
  SHA256_Final(outp, &c);
  return 0;
}

// returns length of null terminated character array
int lengthof(char* a)
{
  int res = 0;
  while(a[res] != '\0')
  {
    res++;
  }
  return res;
}

// generates a random string of len characters using all uppercase letters
int randString(char* dest, int len)
{    
  for(int i = 0; i < len; i++)
  {
    dest[i] = (char) (rand() % 26 + 'A');
  }
  return 0;
}

// calculates result, such that SHA256(inp + result) begins with 2 bytes of '0's
// note 0x0000ffff is used because it is stored with the bytes reversed.
int findNonce(char* inp, int length, unsigned char* result, int* attempts, int* interrupt)
{
  *attempts = 0;
  while(*interrupt == -1)
  {
    *attempts += 1;
    randString(inp + length, 100 - length);
    my_sha(inp, result, 100);
    if( ( * (int*) result & 0x0000ffff ) == 0) break;
  }
  return 0;
}

// initializes and returns a new block.
my_block* createBlock(int miner, char* nonce, unsigned char* hash, my_block* next)
{
  my_block* new_block = (my_block*) malloc(sizeof(my_block));
  new_block->miner = miner;
  new_block->nonce = nonce;
  new_block->next = next;
  new_block->hash = hash;
  return new_block;
}

// converts binary representation to a string of hexadecimal characters.
int bitsToString(unsigned char* inp, int length, char* outp)
{
  for(int i = 0; i < length; i++)
  {
    sprintf(outp + i * 2, "%02x", inp[i]);
  }
  //outp[length*2 + 1] = '\0';
  return 0;
}

// converts non-negative integer to a string
int intToString(int a, char* outp)
{
  int length = 1;
  int temp = a;
  while(1)
  {
    if(temp < 10) break;
    temp /= 10;
    length++;
  }

  for(; length > 0; length--)
  {
    outp[length-1] = (char) (a % 10 + 48);
    a /= 10;
  }
  return 0;
}

// previous is a pointer to the last block mined. return is the next block in the chain
// interrupt added in exercise 5 to interrupt current thread so new chain can be verified
my_block * mineTheNextBlock(my_block* previous, int miner, int* interrupt)
{
  if(*interrupt != -1) return NULL;
  char* inp = (char*) malloc (100 * sizeof(char));
  char* nonce;
  unsigned char* hash = (unsigned char*) malloc (SHA256_DIGEST_LENGTH);
  int* tries = (int*) malloc(sizeof(int));
  if(!previous)
  {
    intToString(miner, inp);
    findNonce(inp, 1, hash, tries, interrupt);
    if(*interrupt != -1) return NULL;
    return createBlock(miner, inp + 1, hash, NULL);
  }
  bitsToString(previous->hash, 32, inp);
  intToString(miner, inp + 64);
  findNonce(inp, 65, hash, tries, interrupt);
  if(*interrupt != -1) return NULL;
  my_block* res = createBlock(miner, inp + 65, hash, NULL);
  previous->next = res;
  return res;
}

// mines n blocks after previous. return is the last block produced
my_block* mineNBlocks(my_block* previous, int miner, int n, int* interrupt)
{
  my_block* last = previous;
  for(int i = 0; i < n; i++)
  {
    if(*interrupt != -1) break;
    last = mineTheNextBlock(last, miner, interrupt);
  }
  return last;
}

// returns length of the chain if valid. returns 0 if not. genesis must be the genesis block
int verifyChain(my_block* genesis)
{
  if(!genesis) return 0;
  int length = 1;
  my_block* next = genesis;
  unsigned char* hash = (unsigned char*) malloc(32);
  char* inp = (char*) malloc(100);
  intToString(genesis->miner, inp);
  strncpy(inp+1, genesis->nonce, 99);
  my_sha(inp, hash, 100);
  if(* (int*) hash & 0x00001111) return 0;
  for(int i = 0; i < 32; i++)
  {
    if(* (hash+i) != *(genesis->hash + i)) return 0;
  }
  next = genesis->next;

  while(next)
  {
    length++;
    bitsToString(hash, 32, inp);
    intToString(next->miner, inp+64);
    strncpy(inp+65, next->nonce, 35);
    my_sha(inp, hash, 100);
    if(* (int*) hash & 0x00001111) return 0;
    for(int i = 0; i < 32; i++)
    {
      if(* (hash+i) != *(next->hash + i)) return 0;
    }
    next = next->next;
  }
  return length;
}

// deep copy of a chain
my_block* copyChain(my_block* genesis)
{
  if(!genesis) return NULL;
  my_block* copy = createBlock(genesis->miner, genesis->nonce, genesis->hash, NULL);
  my_block* next = copy;
  while(genesis->next)
  {
    genesis = genesis->next;
    next->next = createBlock(genesis->miner, genesis->nonce, genesis->hash, NULL);
    next = next->next;
  }
  return copy;
}
