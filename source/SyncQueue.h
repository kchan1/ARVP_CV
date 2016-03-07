#include <time.h>
#include <Queue.h>
 template<class Key,class Elem>
   class SyncQueue
 {
 private:
   template<class Key,class Elem>
     class SubQueueList
   {
   public:
     Key subqueue_key;
     clock_t latest;
     SubQueueList<Key,Elem>* next_list;
     Queue<Elem>* subqueue;
     void enqueue(clock_t latest,Elem* e)
     {
       subqueue->enqueue(latest,e);
       latest = subqueue->getLatest();
     }
     Elem* dequeue(void)
     {
       Elem* e = subqueue->dequeue();
       latest = subqueue->getLatest();
     }
   };
   clock_t latest;
   SubQueueList* subqueue_list_head;
 public:
   pthread_mutex_t* QueueMutex;
   clock_t getLatestTime(void);
   void addSubQueue(Key);
   void enqueue(Key,clock_t,Elem*);
   Elem* dequeue(Key);
 };
