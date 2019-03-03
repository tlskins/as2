#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

typedef struct {
	int*		value;
} buffer_item;

class BufferQueue {
  private:
    buffer_item *cqueue_arr;
    int front, rear;
    int MAX;
  public:
    BufferQueue(int max);
    void insert_item(buffer_item item);
    void remove_item();
    void display();
};

#endif
