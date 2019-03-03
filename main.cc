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
bool running = true;

/* Consumer */
void *waiter(void *threadid){
  int sleep_for;

  do {
    pthread_mutex_lock( &mtx );

    if (meal > 0) {
      printf("Waiter %ld) waiting meal... %d meals still pending\n",(long)threadid + 1, meal-1);
      meal--;
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
    }
    else {
      sleep_for = rand() % 4;
      printf("Waiter %ld) no meals. sleeping for %d\n",(long)threadid + 1, sleep_for);
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
      sleep(sleep_for);
    }
  } while(running);

  printf("Waiter %ld) done running\n",(long)threadid + 1);
  pthread_cond_signal( &cv );
  pthread_mutex_unlock( &mtx );
}

/* Producer */
void *makeMeal(void *threadid){
  int make_meal;
  int sleep_for;

  do {
    pthread_mutex_lock( &mtx );
    pthread_cond_wait( &cv, &mtx );
    make_meal = rand() % 2;
    if (make_meal) {
      printf("Chef %ld) making meal %d\n",(long)threadid + 1, meal + 1);
      meal++;
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
    }
    else {
      sleep_for = rand() % 4;
      printf("Chef %ld) too sleepy... sleeping for %d\n",(long)threadid + 1, sleep_for);
      pthread_cond_signal( &cv );
      pthread_mutex_unlock( &mtx );
      sleep(sleep_for);
    }
  } while(running);

  printf("Chef %ld) done running\n",(long)threadid + 1);
  // need to signal other producers waiting for signals
  pthread_cond_signal( &cv );
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
  // for (long order = 0; order < CHEF_COUNT; order++){
  //   pthread_join( chefs[order], NULL);
  // }
  // for (long order = 0; order < WAITER_COUNT; order++){
  //   pthread_join( waiters[order], NULL);
  // }

  printf("main sleeping...");
  sleep(2);
  printf("main DONE sleeping...");

  running = false;
  pthread_exit(NULL);
}
