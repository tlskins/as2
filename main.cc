/**
 * Producer-Comsumer example, written in C++ May 4, 2014
 * Compiled on OSX 10.9, using:
 * g++ -std=c++11 producer_consumer.cpp
 **/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int meal = 0;
int CHEF_COUNT = 10;
int WAITER_COUNT = 2;

/* Consumer */
void *waiter(void *threadid){
  sleep(1);
  while(meal > 0) {
    pthread_mutex_lock( &mtx );
    printf("Waiter %ld processing meal... %d meals still pending\n",(long)threadid + 1, meal-1);
    meal--;
    pthread_cond_signal( &cv );
    pthread_mutex_unlock( &mtx );
  }
  printf("Waiter %ld finished!\n",(long)threadid + 1);
}

/* Producer */
void *makeMeal(void *threadid){
  pthread_mutex_lock( &mtx );
  pthread_cond_wait( &cv, &mtx );
  printf("Make meal %d\n", meal + 1);
  meal++;
  pthread_cond_signal( &cv );
  pthread_mutex_unlock( &mtx );
}

int main(){
  int rc;
  pthread_t chefs[CHEF_COUNT];
  pthread_t waiters[WAITER_COUNT];

  /* Initialize producers */
  for (long order = 0; order < CHEF_COUNT; order++){
    printf("main() : creating producer thread for order, %ld\n", order);
    rc = pthread_create(&chefs[order], NULL, makeMeal, (void *)order);
    if (rc) {
      printf("Error:unable to create producer thread, %d\n", rc);
      exit(-1);
    }
  }

  /* Initialize consumers */
  for (long order = 0; order < WAITER_COUNT; order++){
    printf("main() : creating consumer thread for order, %ld\n", order);
    rc = pthread_create(&waiters[order], NULL, waiter, (void *)order);
    if (rc) {
      printf("Error:unable to create consumer thread, %d\n", rc);
      exit(-1);
    }
  }

  pthread_cond_broadcast(&cv);
  for (long order = 0; order < CHEF_COUNT; order++){
    pthread_join( chefs[order], NULL);
  }
  for (long order = 0; order < WAITER_COUNT; order++){
    pthread_join( waiters[order], NULL);
  }

  pthread_exit(NULL);
}
