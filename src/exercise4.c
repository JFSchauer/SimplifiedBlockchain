#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "SHA.h"

char** messages;
void* message(void* args)
{
  int my_id = * (int*) args;
  messages[my_id] = (char*) malloc(8);
  randString(messages[my_id], 7);
  messages[my_id][7] = '\0';
  printf("Thread %d sending message: %s\n", my_id, messages[my_id]);

  int read[] = {0, 0, 0};
  read[my_id] = 1;
  int finished = 0;

  while(!finished)
  {
    for(int i = 0; i < sizeof(read) / 4; i++)
    {
      if( ! read[i] )
      {
        if(messages[i] != NULL)
        {
          printf("Thread %d reads: %s\n", my_id, messages[i]);
          read[i] = 1;
        }
      }
    }
    for(int i = 0 ; i < sizeof(read) / 4; i++)
    {
      if(read[i] == 0)
      {
        finished = 0;
        break;
      }
      finished = 1;
    }
    sleep(0);
  }

  return NULL;
}

int main()
{
  srand(time(NULL));
  pthread_t* threads = (pthread_t*) malloc(3*sizeof(pthread_t));
  int* ids = (int*) malloc(3 * sizeof(int));
  messages = (char**) malloc(3 * sizeof(char*));
  for(int i = 0; i < 3; i++)
  {
    messages[i] = NULL;
    ids[i] = i;
    pthread_create(threads+i, NULL, message, (void*) (ids + i));
  }
  for(int i = 0; i < 3; i++)
  {
    pthread_join(threads[i], NULL);
  }
  return 0;

}