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
  while(*shared_data->killbit==0)
  {
    time(&later);
    if(difftime(later,now)>=5)
    {
      time(&now);
      if(shared_data->control_table->ctrl_screen->tryLock())
      {
	printf("Blip!\n");
	shared_data->control_table->ctrl_screen->unlock();
      }
    }
    usleep(200);
  }
  printf("THREAD HAS DIED!!! KILLBIT: %i\n",*shared_data->killbit);
  pthread_exit(NULL);
}
#endif
