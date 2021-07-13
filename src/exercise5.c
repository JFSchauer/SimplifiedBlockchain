#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "SHA.h"

my_block** blocks;
int* interrupts;

void* mine(void* args)
{
  int my_id = * (int*) args;
  int my_length;
  my_block* next = NULL;
  while(1)
  {
    if(blocks[my_id])
    {
      next = blocks[my_id];
      while(next->next) next = next->next;
    }
    next = mineTheNextBlock(next, my_id, interrupts + my_id);
    if(!blocks[my_id])
      blocks[my_id] = next;

    if(interrupts[my_id] == -1)
    { // this thread found a nonce, broadcast to all other chains by setting
     // setting their interrupts to my_id
      for(int i = 0; i < 10; i++)
      {
        interrupts[i] = my_id;
      }
      interrupts[my_id] = -1;
    }
    else
    { // another thread found a nonce, verify and if that chain is longer than this
      // one's, accept it
      if(verifyChain(blocks[interrupts[my_id]]) > verifyChain(blocks[my_id]))
      {
        blocks[my_id] = copyChain(blocks[interrupts[my_id]]);
        interrupts[my_id] = -1;
      }
    }
    if(verifyChain(blocks[my_id]) >= 10) break;
  }


  return NULL;
}

int main()
{
  srand(time(NULL));
  pthread_t* threads = (pthread_t*) malloc(10*sizeof(pthread_t));
  int* ids = (int*) malloc(10 * sizeof(int));
  blocks = (my_block**) malloc(10 * sizeof(my_block*));
  interrupts = (int*) malloc(10 * sizeof(int));
  for(int i = 0; i < 10; i++)
  {
    blocks[i] = NULL;
    interrupts[i] = -1;
    ids[i] = i;
    pthread_create(threads+i, NULL, mine, (void*) (ids + i));
  }
  for(int i = 0; i < 10; i++)
  {
    pthread_join(threads[i], NULL);
  }

  my_block* temp = blocks[0];
  char* hash = (char*) malloc(64);
  for(int i = 0; i < 10; i++)
  {
    bitsToString(temp->hash, 32, hash);
    printf("Block %d:\n  Miner: %d\n  Nonce: %s\n  Hash: %s\n",
      i, temp->miner, temp->nonce, hash);
    temp = temp->next;
  }

  return 0;
}