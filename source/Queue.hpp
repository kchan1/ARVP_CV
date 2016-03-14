#include <time.h>
template<class Elem>
  class Queue
{
 private:
  template<class Elem>
    class SubQueueNode
    {
      clock_t time;
      SubQueueNode* next;
      SubQueueNode*prev;
      Elem*data;
    };
  int size;
  SubQueueNode* head;
  SubQueueNode* tail;
 public:
  int getSize();
  clock_t getLatest();
  void enqueue(clock_t,Elem*);
  Elem* dequeue(void);
}
