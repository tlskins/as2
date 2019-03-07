#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
// #include "buffer_queue.h"

using namespace std;

// https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032r/index.html#sync-53686
pthread_mutex_t     meal_lock;
pthread_cond_t      meal_nonzero;
unsigned            meal = 0;
int                 CHEF_COUNT = 4;
int                 WAITER_COUNT = 10;
int                 MAX_MEALS = 5;
bool                running = true;

/* Producer */
void *makeMeal(void *threadid){
  int       make_meal;
  unsigned  meal_id = 0;

  do {
    // make_meal is local variable so we don't need to protect by mutex
    make_meal = rand() % 2;

    if (make_meal){
      // lock meal
      pthread_mutex_lock( &meal_lock );

      /*
       * We will increment meal count so give a signal beforehand
       * if meail is zero because other threads may wait.
       * It doesn't matter before increment meal here or later because
       * meal is still locked
       */
      if (meal == 0){
        pthread_cond_signal( &meal_nonzero );
      }
      if (meal < MAX_MEALS) {
        meal++;
        meal_id = meal;
      }
      pthread_mutex_unlock( &meal_lock );

      /*
       * during locking of mutex avoiding any I/O because it is very expensive and slowdown
       * all threads, we carry meal_id here to do the I/O
       */
      if (meal_id != 0){
        printf("Chef %ld) making meal %d\n", (long)threadid + 1, meal_id + 1);
      }
    }
    else {
      int   sleep_for = rand() % 3 + 1;
      // printf("Chef %ld), sleeping for %d\n", (long)threadid + 1, sleep_for);
      sleep(sleep_for);
    }
  } while(running);

  printf("Chef %ld) done running\n", (long)threadid + 1);
  return NULL;
}

/* Consumer */
void *waiter(void *threadid){
  unsigned    meal_id;
  int         sleep_for;
  do {

    meal_id = 0;
    sleep_for = rand() % 3 + 1;
    // printf("Waiter %ld), sleeping for %d\n",(long)threadid + 1, sleep_for);
    sleep(sleep_for);

    pthread_mutex_lock( &meal_lock );

    while (meal == 0 && running == true){
      pthread_cond_wait( &meal_nonzero, &meal_lock);
    }
    meal_id = meal;
    meal--;
    pthread_mutex_unlock( &meal_lock );

    if (meal_id != 0){
      printf("Waiter %ld) took meal %d\n",(long)threadid + 1, meal_id + 1);
    }

  } while( running);

  printf("Waiter %ld) done running\n",(long)threadid + 1);
  return NULL;
}


int main(int argc, char* argv[]){
  int rc;
  pthread_t chefs[CHEF_COUNT];
  pthread_t waiters[WAITER_COUNT];

  pthread_mutex_init(&meal_lock, 0);
  pthread_cond_init(&meal_nonzero, 0);

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

  /*
   * https://www.ibm.com/support/knowledgecenter/en/ssw_aix_71/com.ibm.aix.basetrf1/pthread_cond_signal.htm
   *
   * The pthread_cond_signal subroutine unblocks at least one blocked thread,
   * while the pthread_cond_broadcast subroutine unblocks all the blocked threads.
   * 
   * producer hasn't produce anything yet so we should not call pthread_cond_broadcast() here
   */
  // pthread_cond_broadcast(&cv);

  printf("main() sleeping...\n");
  sleep(20);
  printf("main() DONE sleeping...\n");
  running = false;
  
  // some threads may wait for meal so singal all threads
  pthread_mutex_lock( &meal_lock);
  pthread_cond_broadcast( &meal_nonzero);
  pthread_mutex_unlock( &meal_lock);

  // we wait until all threads end
  for (long order = 0; order < CHEF_COUNT; order++){
    pthread_join(chefs[order], NULL);
  }
  for (long order = 0; order < WAITER_COUNT; order++){
    pthread_join(waiters[order], NULL);
  }

  pthread_mutex_destroy(&meal_lock);
  pthread_cond_destroy(&meal_nonzero);

  /*
   * we don't need to call pthread_exit() because all other threads already joined
   */
  // pthread_exit(NULL);

  return 0;

}
