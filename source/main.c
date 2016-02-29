/******************************************************************************
* FILE: main.c
* DESCRIPTION:
*   The entry point for the drone logic. Constructs and runs threads.
* AUTHOR: Kennith Chan
* LAST REVISED: 02/28/16
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "SyncQueue.h"
#define NUM_THREADS 5

//holds a configuration
struct Config
{
  int baudrate;
  char port[128]; 
};

//holds the data buffers for the pipeline
struct DataTable
{
  SyncQueue* snapshot_queue;
  SyncQueue* target_queue;
  SyncQueue* results_queue;
  FILE* logfile;
};

//holds pointers to the threads
struct ThreadTable
{
  pthread_t* activate;
  pthread_t* take_picture;
  pthread_t* get_pixhawk;
  pthread_t* target_find;
  pthread_t* calc_target_loc;
  pthread_t* OCR;
  pthread_t* publish;
};

//holds the ready bits (should be accessed through mutex)
struct ReadyTable
{
  short int ready_activate = 0;
  short int ready_take_picture = 0;
  short int ready_get_pixhawk = 0;
  short int ready_target_find = 0;
  short int ready_calc_target_loc = 0;
  short int ready_OCR = 0;
  short int ready_publish = 0;
};

//holds the notify message buffers, either 0 or a clock time
struct NotifyTable
{
  clock_t notify_activate = 0;
  clock_t notify_take_picture = 0;
  clock_t notify_get_pixhawk = 0;
  clock_t notify_target_find = 0;
  clock_t notify_calc_target_loc = 0;
  clock_t notify_OCR = 0;
  clock_t notify_publish = 0;
};

//holds mutexes not held by the queues
struct MiscMutex
{
  pthread_mutex_t* activate_mutex;
  pthread_mutex_t* screen_mutex;
}

//packs all the data for all threads to access
struct SharedData {
  struct Config* config;
  struct DataTable* data_table;
  struct ThreadTable* thread_table;
  struct ReadyTable* ready_table;
  struct NotifyTable* notify_table;
  int* killbit;
}
  
void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  //grab configuration here
  struct Config* config = new Config();
  
  //construct data structures here, point to them
  struct DataTable* data_table = new DataTable();

  //construct sync mechanisms, point to them
  struct ReadyTable* ready_table = new ReadyTable();
  struct NotifyTable* notify_table = new NotifyTable();
  struct MiscMutex* misc_mutex = new MiscMutex();
  pthread_mutex_t* activate_mutex = new pthread_mutex_t();
  pthread_mutex_t* screen_mutex = new pthread_mutex_t();
  misc_mutex->activate_mutex = activate_mutex;
  misc_mutex->screen_mutex = screen_mutex;
  int killbit = 0;

  //construct threads, point to them
  struct ThreadTable* thread_table = new ThreadTable();
  
  //pack all data for threads to share
  struct SharedData* shared_data = new SharedData();
  shared_data->config = config;
  shared_data->data_table = data_table;
  shared_data->thread_table = thread_table;
  shared_data->ready_table = ready_table;
  shared_data->notify_table = notify_table;
  shared_data->killbit = &killbit;

  //execute threads

  //poll on user input
  char user_in[256]="";
  int screenstate = 0;
  do
  {
    printf("Enter 'cmd' to enter a command, or 'exit' to exit\n");
    scanf("%s",&user_in);

    //cases for command
    if(user_in=="cmd")
    {
      int done = 0;
      //lock the screen so that publish doesn't interrupt
      pthread_mutex_lock(screen_mutex);
      printf("Enter your command, or nothing to stop\n");
      while(!done)
      {
	scanf("%s",&user_in);

	if(user_in=="help")
	{
	  printf("Current Commands: snap")
	}
	else if(user_in=="snap")
	{
	  pthread_mutex_lock(shared_data->misc_mutex->activate_mutex);
	  notify_activate = clock();
	  pthread_mutex_unlock(shared_data->misc_mutex->activate_mutex);
	}
      }
      pthread_mutex_unlock(screen_mutex);
    }
    //case for status
    else if(user_in=="stat")
    {
      //lock each queue consecutively to get the sizes, then lock screen and print screen
    }
  }while(user_in!="exit");
  //exit
  killbit=1;
  //pthread_exit(NULL);
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
  for(t=0;t<NUM_THREADS;t++){
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  
  /* Last thing that main() should do */
  pthread_exit(NULL);
}
