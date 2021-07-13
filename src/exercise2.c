#include "SHA.h"


int miner = 0;
int main()
{
  int interrupt = -1;
  my_block* genesis = mineTheNextBlock(NULL, miner, &interrupt);
  my_block* last = genesis;
  mineNBlocks(last, miner, 9, &interrupt);

  last = genesis;
  char* hash = (char*) malloc(64);
  for(int i = 0; i < 10; i++)
  {
    bitsToString(last->hash, 32, hash);
    printf("Block %d:\n  Miner: %d\n  Nonce: %s\n  Hash: %s\n",
      i, last->miner, last->nonce, hash);
    last = last->next;
  }

  printf("Calling verifyChain(genesis)...\n");

  if( verifyChain(genesis) ) printf("Chain is valid.\n");
  else printf("Chain is not valid.\n");

  return 0;
}