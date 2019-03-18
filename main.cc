#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "buffer_queue.h"
#include <semaphore.h>

using namespace std;

int MAX_BUFFER_SIZE = 5;
int PRODUCER_COUNT;
int CONSUMER_COUNT;
int MAIN_SLEEP_FOR;
sem_t *empty, *full, *mtx;
bool running = true;
BufferQueue queue(MAX_BUFFER_SIZE);

/* Consumer */
void *consumer(void *threadid){
  int sleep_for;

  do {
    sleep_for = rand() % 3 + 1;
    printf("Consumer %ld) sleeping for %d\n",(long)threadid + 1, sleep_for);
    sleep(sleep_for);
    printf("Consumer %ld) waiting full...\n",(long)threadid + 1);
    sem_wait(full); // wait if full semaphore = 0
    printf("Consumer %ld) waiting mtx...\n",(long)threadid + 1);
    sem_wait(mtx); // decrement mtx and acquire lock

    // consume and dequeue
    queue.remove_item();
    printf("Consumer %ld) finished consuming. ",(long)threadid + 1);
    queue.display();

    sem_post(mtx); // increment mtx and release lock
    sem_post(empty); // increment empty semaphore
  } while(running);

  printf("Consumer %ld) done running\n",(long)threadid + 1);
  return NULL;
}

/* Producer */
void *producer(void *threadid){
  int produce, sleep_for;

  do {
    produce = rand() % 2;
    if (produce) {
      printf("Producer %ld) waiting empty...\n",(long)threadid + 1);
      sem_wait(empty); // wait if empty semaphore = 0
      printf("Producer %ld) waiting mtx...\n",(long)threadid + 1);
      sem_wait(mtx); // decrement mtx and acquire lock

      // produce and enqueue
      buffer_item item = { rand() % RAND_MAX + 1 };
      queue.insert_item(item);
      printf("Producer %ld) produced %d\n",(long)threadid + 1, item.getValue());

      sem_post(mtx); // increment mtx and release lock
      sem_post(full); // increment full semaphore
    }
    else {
      sleep_for = rand() % 3 + 1;
      printf("Producer %ld) sleeping for %d\n",(long)threadid + 1, sleep_for);
      sleep(sleep_for);
    }
  } while(running);

  printf("Producer %ld) done running\n",(long)threadid + 1);
  return NULL;
}

int main(){
  int rc;

  // get user input for how long prog should run, num of producers and consumers
  printf("Number of seconds to sleep before terminating> ");
  fflush(stdout);
  cin >> MAIN_SLEEP_FOR;

  printf("Number of producer threads> ");
  fflush(stdout);
  cin >> PRODUCER_COUNT;

  printf("Number of consumer threads> ");
  fflush(stdout);
  cin >> CONSUMER_COUNT;

  // initialize pthread arrays
  pthread_t producers[PRODUCER_COUNT];
  pthread_t consumers[CONSUMER_COUNT];

  //remove existing named semaphores
  sem_unlink("/empty");
  sem_unlink("/full");
  sem_unlink("/mtx");

  // initialize semaphores
  empty = sem_open("/empty", O_CREAT|O_EXCL, S_IRWXU, MAX_BUFFER_SIZE);
  full = sem_open("/full", O_CREAT|O_EXCL, S_IRWXU, 0);
  mtx = sem_open("/mtx", O_CREAT|O_EXCL, S_IRWXU, 1);

  // create producers
  for (long order = 0; order < PRODUCER_COUNT; order++){
    printf("main() : creating producer thread with id: %ld\n", order);
    rc = pthread_create(&producers[order], 0, producer, (void *)order);
    if (rc) {
      printf("Error:unable to create producer thread, %d\n", rc);
      exit(-1);
    }
  }

  // create consumers
  for (long order = 0; order < CONSUMER_COUNT; order++){
    printf("main() : creating consumer thread with id: %ld\n", order);
    rc = pthread_create(&consumers[order], 0, consumer, (void *)order);
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
  return 0;
}
