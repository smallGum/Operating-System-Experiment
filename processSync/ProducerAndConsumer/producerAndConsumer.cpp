#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

#define BUFFER_SIZE 5
typedef int buffer_item;

// thread parameter structure
struct parameters {
  int start_time, end_time;
  buffer_item item;
};

// buffer structure
struct sto {
  buffer_item buffer[BUFFER_SIZE];
  sem_t mutex, empty, full;
};

struct sto shared;
int nthreads, current_pro = 0;

// thread functions
void *produce(void *);
void *consume(void *);

int main(int argc, char **argv)
{
  // read the number of threads
  ifstream fin("test_data.txt");
  if (!fin.is_open()) { 
    cout << "cannot open the file!" << endl; 
    exit(1);
  }
  fin >> nthreads;
  pthread_t *tids = new pthread_t[nthreads];

  // initialize the semaphores
  sem_init(&shared.mutex, 0, 1);
  sem_init(&shared.empty, 0, BUFFER_SIZE);
  sem_init(&shared.full, 0, 0);

  // create the threads to produce or consume products in the buffer
  for (int i = 0; i < nthreads; ++i) {
    // read the data of current thread
    char role;
    struct parameters* paras = new struct parameters[1];
    fin >> tids[i] >> role >> paras -> start_time >> paras -> end_time;

    if (role == 'P') {
      fin >> paras -> item;
      pthread_create(&tids[i], NULL, produce, (void *)paras);
      sleep(1);
    } 
    else if (role == 'C') {
      paras -> item = -1;
      pthread_create(&tids[i], NULL, consume, (void *)paras);
      sleep(1);
    }
  }

  // join the threads
  for (int i = 0; i < nthreads; ++i) {
  	pthread_join(tids[i], NULL);
  }
  // destroy the semaphore
  sem_destroy(&shared.mutex);
  sem_destroy(&shared.empty);
  sem_destroy(&shared.full);
  // close the file
  fin.close();

  return 0;
}

void *produce(void *paras)
{
  struct parameters* tparas = (struct parameters*) paras;
  cout << "new producer thread created. start time: " << tparas -> start_time << ' ' << "end time: " << tparas -> end_time << endl;

  // wait to start
  sleep(tparas -> start_time);
  cout << "new producer apply for producing..." << endl;

  sem_wait(&shared.empty);
  sem_wait(&shared.mutex);
  // add the item to the buffer
  shared.buffer[current_pro] = tparas -> item;
  cout << "Producing " << shared.buffer[current_pro]  << "..." << endl;
  // wait to finish
  sleep(tparas -> end_time);
  cout << "Produced" << shared.buffer[current_pro] << endl;
  current_pro = (current_pro + 1) % BUFFER_SIZE;
  sem_post(&shared.mutex);
  sem_post(&shared.full);

  free(tparas);
}

void *consume(void *paras)
{
  //cout << "new consumer thread created." << endl;
  struct parameters* tparas = (struct parameters*) paras;
  cout << "new consumer thread created. start time: " << tparas -> start_time << ' ' << "end time: " << tparas -> end_time << endl;

  // wait to start
  sleep(tparas -> start_time);
  cout << "new consumer apply for comsuming..." << endl;

  sem_wait(&shared.full);
  sem_wait(&shared.mutex);
  // remove the item of the buffer
  current_pro = (current_pro + 4) % BUFFER_SIZE;
  cout << "Consuming " << shared.buffer[current_pro] << "..." << endl;
  // wait to finish
  sleep(tparas -> end_time);
  cout << "Consumed " << shared.buffer[current_pro] << endl;
  sem_post(&shared.mutex);
  sem_post(&shared.empty);

  free(tparas);
}
