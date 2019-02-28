/**
 * Producer-Comsumer example, written in C++ May 4, 2014
 * Compiled on OSX 10.9, using:
 * g++ -std=c++11 producer_consumer.cpp
 **/

#include <iostream>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cv;
bool ready = false;         // Tell threads to run
int meal = 0;

/* Consumer */
void *waiter(void *threadid){
  unique_lock<mutex> lck(mtx);
  while(meal == 0 || !ready) cv.wait(lck);
  cout << "Order: ";
  cout << (long)threadid + 1 << " being taken care of with ";
  cout << meal - 1 << " meals also ready." << endl;
  meal--;
}

/* Producer */
void *makeMeal(void *threadid){
  unique_lock<mutex> lck(mtx);
  while(!ready) cv.wait(lck);
  cout << "Make meal " << meal + 1 << endl;
  meal++;
  cv.notify_one();
}

/* Changes ready to true, and begins the threads printing */
void run(){
  cout << "run called" << endl;
  unique_lock<mutex> lck(mtx);
  ready = true;
  cv.notify_all();
}

int main(){
  int rc1, rc2;
  pthread_t chefs[10];
  pthread_t waiters[10];

  /* Initialize customers and chefs */
  for (long order = 0; order < 10; order++){
    cout << "main() : creating threads for order, " << order << endl;
    rc1 = pthread_create(&chefs[order], NULL, makeMeal, (void *)order);
    rc2 = pthread_create(&waiters[order], NULL, waiter, (void *)order);

    if (rc1) {
       cout << "Error:unable to create producer thread, " << rc1 << endl;
       exit(-1);
    }
    if (rc2) {
       cout << "Error:unable to create consumer thread, " << rc2 << endl;
       exit(-1);
    }
  }
  run();

  pthread_exit(NULL);
}
