#ifndef _SHARE_HPP_
#define _SHARE_HPP_
#include "threading.hpp"
//holds a configuration
struct Config
{
  //Startup info
  time_t trigger_start;
  time_t trigger_speed;
  double shutter_speed;
  //Targeting info
  double blur_size;
  unsigned int smooth_size;
  unsigned int gradient_thres_up;
  unsigned int gradient_thres_down;
  const char* kmeans_file_name;
  const char* image_directory;
};

//holds the data buffers for the pipeline
struct DataTable
{
  //SyncQueue* snapshot_queue;
  //SyncQueue* target_queue;
  //SyncQueue* results_queue;
  FILE* logfile;
  ~DataTable()
  {
    ;
  }
};

//holds pointers to the threads
struct ThreadTable
{
  pthread_t* main;
  pthread_t* activate;
  pthread_t* take_picture;
  pthread_t* target_find;
  pthread_t* publish;
};

//holds the thread control objects
struct ControlTable
{
  ThreadControl* ctrl_main;
  ThreadControl* ctrl_activate;
  ThreadControl* ctrl_take_picture;
  ThreadControl* ctrl_target_find;
  ThreadControl* ctrl_publish;
  ThreadControl* ctrl_screen;
  ~ControlTable()
  {
    delete ctrl_main;
    delete ctrl_activate;
    delete ctrl_take_picture;
    delete ctrl_target_find;
    delete ctrl_publish;
    delete ctrl_screen;
  }
};

//packs all the data for all threads to access
struct SharedData {
  struct Config* config;
  struct DataTable* data_table;
  struct ThreadTable* thread_table;
  struct ControlTable* control_table;
  int* killbit;
  ~SharedData()
  {
    delete config;
    delete data_table;
    delete thread_table;
    delete control_table;
    delete killbit;
  }
};
#endif
