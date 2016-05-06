/******************************************************************************
* FILE: main.c
* DESCRIPTION:
*   The entry point for the drone logic. Constructs and runs threads.
* AUTHOR: Kennith Chan
* LAST REVISED: 05/6/16
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#include "SyncQueue.h"
#include "share.hpp"
#include "Dummy/dummy.hpp"
#include "threading.hpp"
#include <string.h>
#define NUM_THREADS 5

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  printf("Starting program\n");
  printf("Creating Structs\n");
  //grab configuration here
  struct Config* config = new Config();
  
  //construct data structures here, point to them
  struct DataTable* data_table = new DataTable();

  //construct sync/control structures, point to them
  struct ControlTable* control_table = new ControlTable();
  control_table->ctrl_main=new ThreadControl();
  control_table->ctrl_activate=new ThreadControl();
  control_table->ctrl_take_picture=new ThreadControl();
  control_table->ctrl_publish=new ThreadControl();
  control_table->ctrl_screen=new ThreadControl();

  //construct this now, fill it out later
  struct ThreadTable* thread_table = new ThreadTable();
  
  //pack all data for threads to share
  struct SharedData* shared_data = new SharedData();
  shared_data->config = config;
  shared_data->data_table = data_table;
  shared_data->thread_table = thread_table;
  shared_data->control_table = control_table;
  shared_data->killbit = new int();
  *shared_data->killbit = 0;
  printf("Finished Shared Data Table\n");
  
  //construct and execute threads, point to them in the table
  //shared_data->thread_table->main = new pthread_t();
  //*shared_data->thread_table->main = pthread_self();
  /*
  pthread_create(shared_data->thread_table->activate,NULL, activate,(void*)shared_data);
  pthread_create(shared_data->thread_table->take_picture,NULL, take_picture,(void*)shared_data);
  pthread_create(shared_data->thread_table->target_find,NULL, target_find,(void*)shared_ata);
  pthread_create(shared_data->thread_table->publish,NULL, publish,(void*)shared_data);  
  */
  shared_data->thread_table->activate = new pthread_t();
  shared_data->thread_table->take_picture = new pthread_t();
  shared_data->thread_table->target_find = new pthread_t();
  shared_data->thread_table->publish = new pthread_t();
  
  printf("Starting Activate\n");
  pthread_create(shared_data->thread_table->activate,NULL, dummy,(void*)shared_data);
  printf("Starting TakePicture\n");
  pthread_create(shared_data->thread_table->take_picture,NULL, dummy,(void*)shared_data);
  printf("Starting TargetFind\n");
  pthread_create(shared_data->thread_table->target_find,NULL, dummy,(void*)shared_data);
  printf("Starting Publish\n");
  pthread_create(shared_data->thread_table->publish,NULL, dummy,(void*)shared_data);  

  printf("Polling for user input now\n");
  //poll on user input
  char user_in[256]="";
  do
  {
    printf("Enter 'cmd' to enter a command, 'status' for how I'm doing, or 'q' to exit\n");
    scanf("%s",user_in);
    printf("Got '%s'\n",user_in);
    //cases for command
    if(strcmp(user_in,"cmd")==0)
    {
      int done = 0;
      //lock the screen so that publish doesn't interrupt
      control_table->ctrl_screen->waitLock();
      printf("Screen Locked, Entering Command Mode,'help' for help, 'return' to stop\n");
      while(!done)
      {
	scanf("%s",user_in);
	printf("Got '%s'\n",user_in);
	if(strcmp(user_in,"help")==0)
	{
	  printf("Current Commands: help, snap\n");
	}
	else if(strcmp(user_in,"snap")==0)
	{
	  printf("Manual Snapshot!\n");
	  control_table->ctrl_take_picture->waitLock();
	  ThreadMessage*msg = new ThreadMessage(0x1,clock(),NULL);
	  control_table->ctrl_take_picture->addMessage(msg);
	  control_table->ctrl_take_picture->wake();
	  control_table->ctrl_take_picture->unlock();
	}
	else if(strcmp(user_in,"return")==0)
	{
	  done = 1;
	}
      }
      printf("Unlocking Screen, exiting command mode...\n");
      control_table->ctrl_screen->unlock();
    }
    //case for status
    else if(strcmp(user_in,"status")==0)
    {
      LinkedList<ThreadMessage>* msg_buff = new LinkedList<ThreadMessage>();
      //lock each queue consecutively to get the sizes, then lock screen and print screen
      control_table->ctrl_screen->waitLock();
      printf("--------SYSTEM STATUS--------\n");
      printf("-thread 'activate':\n");
      printf("-\tstatus     : %04x\n",control_table->ctrl_activate->getStatus());
      printf("-\thasMessage: %s\n",control_table->ctrl_activate->hasMessage()?"true":"false");
      while(control_table->ctrl_activate->hasMessage())
      {
	ThreadMessage*msg = control_table->ctrl_activate->nextMessage();
	printf("-\tMessage: flags:%04u clock:%u\n",msg->getFlags(),(unsigned int)msg->getTime());
	msg_buff->push(msg);
      }
      while(msg_buff->hasNext())
      {
	ThreadMessage*msg = msg_buff->shift();
	control_table->ctrl_activate->addMessage(msg);
      }

      printf("-thread 'take_picture':\n");
      printf("-\tstatus     : %04x\n",control_table->ctrl_take_picture->getStatus());
      printf("-\thasMessage: %s\n",control_table->ctrl_take_picture->hasMessage()?"true":"false");
      while(control_table->ctrl_take_picture->hasMessage())
      {
	ThreadMessage*msg = control_table->ctrl_take_picture->nextMessage();
	printf("-\tMessage: flags:%04u clock:%u\n",msg->getFlags(),(unsigned int)msg->getTime());
	msg_buff->push(msg);
      }
      while(msg_buff->hasNext())
      {
	ThreadMessage*msg = msg_buff->shift();
	control_table->ctrl_take_picture->addMessage(msg);
      }
      printf("-----------------------------\n");
      control_table->ctrl_screen->unlock();     
    }
  }while(!strcmp(user_in,"q")==0);
  //exit
  *shared_data->killbit=1;
  //join all processes
  void* status;
  pthread_join(*shared_data->thread_table->activate,&status);
  pthread_join(*shared_data->thread_table->take_picture,&status);
  pthread_join(*shared_data->thread_table->target_find,&status);
  pthread_join(*shared_data->thread_table->publish,&status);
  //delete all data
  delete shared_data;
  printf("EXITING\n");
  pthread_exit(NULL);
}
