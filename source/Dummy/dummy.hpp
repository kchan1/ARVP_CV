#ifndef _DUMMY_HPP_
#define _DUMMY_HPP_
#include "../share.hpp"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
void *dummy(void* data)
{
  printf("STARTED A DUMMY!\n");
  struct SharedData* shared_data = (SharedData*)data;
  time_t now,later;
  time(&now);
  time(&later);
  while(shared_data->killbit==0)
  {
    time(&later);
    if(difftime(now,later)>=5)
    {
      shared_data->control_table->ctrl_screen->waitLock();
      printf("Blip!\n");
      shared_data->control_table->ctrl_screen->unlock();
    }
    usleep(200);
  }
  pthread_exit(NULL);
}
#endif
