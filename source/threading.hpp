#ifndef _THREADING_HPP_
#define _THREADING_HPP_
#include <pthread.h>
#include <stdio.h>
#include "LinkedList.hpp"

class ThreadMessage
{
private:
  unsigned int flags;
  clock_t time;
  void* data;
public:
  ThreadMessage(unsigned int flags,clock_t time,void* data)
  {
    this->flags = flags;
    this->time = time;
    this-> data = data;
  }
  unsigned int getFlags()
  {return this->flags;}
  clock_t getTime()
  {return this->time;}
  void* getData()
  {return this->data;}
};

class ThreadControl
{
private:
  LinkedList<ThreadMessage>*messages;
  pthread_mutex_t ctrl_mutex;
  pthread_cond_t notify;
  unsigned int status;
public:
  ThreadControl()
  {
    this->messages = new LinkedList<ThreadMessage>();
    pthread_mutex_init(&(this->ctrl_mutex),NULL);
    pthread_cond_init(&(this->notify),NULL);
  }
  ~ThreadControl()
  {
    while(this->messages->hasNext())
    {
      ThreadMessage*next = this->messages->pop();
      delete next;
    }
    pthread_mutex_destroy(&(this->ctrl_mutex));
    pthread_cond_destroy(&(this->notify));
  }
  void wake()
  {pthread_cond_signal(&(this->notify));}
  bool tryLock()
  {return pthread_mutex_trylock(&(this->ctrl_mutex))==0;}
  void waitLock()
  {pthread_mutex_lock(&(this->ctrl_mutex));}
  void unlock()
  {pthread_mutex_unlock(&(this->ctrl_mutex));}
  void addMessage(ThreadMessage*msg)
  {this->messages->push(msg);}
  bool hasMessage()
  {return this->messages->hasNext();}
  ThreadMessage*nextMessage()
  {return this->messages->pop();}
  void setStatus(unsigned int new_status)
  {this->status = new_status;}
  unsigned int getStatus()
  {return this->status;}
};
#endif
