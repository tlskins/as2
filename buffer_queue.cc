/*
 * C++ Program to Implement Circular Queue
 */
#include <iostream>
#include "buffer_queue.h"

using namespace std;

/*
 * Circular Queue implementation logic
 * https://www.sanfoundry.com/cpp-program-implement-circular-queue/
*/
BufferQueue::BufferQueue(int max){
  cqueue_arr = new buffer_item [max];
  MAX = max;
  front = rear = -1;
}

// Insert item to queue
void BufferQueue::insert_item(buffer_item item){
  if ((front == 0 && rear == MAX-1) || (front == rear+1)) {
    cout<<"Queue Overflow \n";
    return;
  }

  // update indices to maintain circular queue
  if (front == -1) {
    front = rear = 0;
  }
  else {
    if (rear == MAX - 1) rear = 0;
    else rear = rear + 1;
  }
  cqueue_arr[rear] = item;
}

// Remove item from queue
void BufferQueue::remove_item(){
  if (front == -1) {
    cout<<"Queue Underflow\n";
    return ;
  }
  cout<<"Element deleted from queue is : "<<cqueue_arr[front].value<<endl;

  // update indices to maintain circular queue
  if (front == rear) {
    front = rear = -1;
  }
  else {
    if (front == MAX - 1) front = 0;
    else front = front + 1;
  }
}

// Print queue elements
void BufferQueue::display() {
  int front_pos = front, rear_pos = rear;
  if (front == -1) {
    cout<<"Queue is empty\n";
    return;
  }
  cout<<"Queue elements: ";
  if (front_pos <= rear_pos) {
    while (front_pos <= rear_pos) {
      cout<<cqueue_arr[front_pos].value<<"  ";
      front_pos++;
    }
  }
  else {
    while (front_pos <= MAX - 1) {
      cout<<cqueue_arr[front_pos].value<<"  ";
      front_pos++;
    }
    front_pos = 0;
    while (front_pos <= rear_pos) {
      cout<<cqueue_arr[front_pos].value<<"  ";
      front_pos++;
    }
  }
  cout<<endl;
}
