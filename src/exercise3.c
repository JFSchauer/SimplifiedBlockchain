#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


void* printTime(void* args)
{
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("thread: %d, %s", * (int*) args, asctime(timeinfo));

  return NULL;
}

int main()
{
  pthread_t* threads = (pthread_t*) malloc(10*sizeof(pthread_t));
  int* ids = (int*) malloc(10 * sizeof(int));
  for(int i = 0; i < 10; i++)
  {
    ids[i] = i;
    printf("%d : ", threads+i);
    int temp = pthread_create(threads+i, NULL, printTime, (void*) (ids + i));
    printf("Created thread %d\n", threads+i);
  }
  for(int i = 0; i < 10; i++)
  {
    pthread_join(threads[i], NULL);
  }
  return 0;

}