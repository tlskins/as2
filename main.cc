#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "buffer_queue.h"

using namespace std;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int MAX_MEALS = 5;
int PRODUCER_COUNT;
int CONSUMER_COUNT;
int MAIN_SLEEP_FOR;
bool running = true;
BufferQueue queue(MAX_MEALS);

/* Consumer */
void *consumer(void *threadid){
  int sleep_for;

  do {
    pthread_mutex_lock( &mtx );

    sleep_for = rand() % 3 + 1;
    printf("Consumer %ld) sleeping for %d\n",(long)threadid + 1, sleep_for);
    sleep(sleep_for);
    while (queue.is_empty() && running) pthread_cond_wait( &cv, &mtx );
    // the condition wait can release when program is done running but queue
    // is empty, in this case we want to check again to avoid underflow
    if (!queue.is_empty()) {
      queue.remove_item();
      printf("Consumer %ld) finished consuming. ",(long)threadid + 1);
      queue.display();
    }
    pthread_cond_signal( &cv );
    pthread_mutex_unlock( &mtx );
  } while(running);

  printf("Consumer %ld) done running\n",(long)threadid + 1);
  return NULL;
}

/* Producer */
void *producer(void *threadid){
  int produce, sleep_for;

  do {
    pthread_mutex_lock( &mtx );
    produce = rand() % 2;
    if (produce) {
      while (queue.is_full() && running) pthread_cond_wait( &cv, &mtx );
      // the condition wait can release when program is done running but queue
      // is full, in this case we want to check again to avoid overflow
      if (!queue.is_full()) {
        buffer_item item = { rand() % RAND_MAX + 1 };
        queue.insert_item(item);
        printf("Producer %ld) produced %d\n",(long)threadid + 1, item.getValue());
      }
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
    }
    else {
      sleep_for = rand() % 3 + 1;
      printf("Producer %ld) sleeping for %d\n",(long)threadid + 1, sleep_for);
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
      sleep(sleep_for);
    }
  } while(running);

  printf("Producer %ld) done running\n",(long)threadid + 1);
  return NULL;
}

int main(){
  int rc;
  pthread_t producers[PRODUCER_COUNT];
  pthread_t consumers[CONSUMER_COUNT];

  printf("Number of seconds to sleep before terminating> ");
  fflush(stdout);
  cin >> MAIN_SLEEP_FOR;

  printf("Number of producer threads> ");
  fflush(stdout);
  cin >> PRODUCER_COUNT;

  printf("Number of consumer threads> ");
  fflush(stdout);
  cin >> CONSUMER_COUNT;

  /* Initialize producers */
  for (long order = 0; order < PRODUCER_COUNT; order++){
    printf("main() : creating producer thread with id: %ld\n", order);
    rc = pthread_create(&producers[order], NULL, producer, (void *)order);
    if (rc) {
      printf("Error:unable to create producer thread, %d\n", rc);
      exit(-1);
    }
  }

  /* Initialize consumers */
  for (long order = 0; order < CONSUMER_COUNT; order++){
    printf("main() : creating consumer thread with id: %ld\n", order);
    rc = pthread_create(&consumers[order], NULL, consumer, (void *)order);
    if (rc) {
      printf("Error:unable to create consumer thread, %d\n", rc);
      exit(-1);
    }
  }

  printf("main() sleeping...\n");
  sleep(MAIN_SLEEP_FOR);
  printf("main() DONE sleeping...\n");

  running = false;
  pthread_exit(NULL);
}
