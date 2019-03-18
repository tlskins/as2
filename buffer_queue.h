#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

// define buffer_item struct for production / consumption
typedef struct {
	int		value;
	int getValue() {
		return value;
	}
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
