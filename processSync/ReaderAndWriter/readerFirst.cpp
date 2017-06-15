#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

// parameters for each thread 
struct parameters {
  int start_time, end_time, tid;
};

int read_count = 0;  // record the number of readers that are reading the file
sem_t mutex;         // reader-reader mutex for read_count
sem_t write_file;    // critial object for writer-writer mutex and reader-writer mutex
int nthreads;        // record the number of threads    

void *reader(void *);
void *writer(void *);

int main()
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
  sem_init(&mutex, 0, 1);
  sem_init(&write_file, 0, 1);

  // create threads to read or write the file
  for (int i = 0; i < nthreads; ++i) {
    char role;
    struct parameters *paras = new struct parameters[1];
    fin >> tids[i] >> role >> paras -> start_time >> paras -> end_time;
    paras -> tid = tids[i];

    if (role == 'R') {
      pthread_create(&tids[i], NULL, reader, (void *)paras);
      sleep(1);
    }
    else if (role == 'W') {
      pthread_create(&tids[i], NULL, writer, (void *)paras);
      sleep(1);
    }
  }

  // join the threads
  for (int i = 0; i < nthreads; ++i) {
    pthread_join(tids[i], NULL);
  }
  // destroy the semaphore
  sem_destroy(&mutex);
  sem_destroy(&write_file);
  // close the file
  fin.close();

  return 0;
}

void *reader(void *paras) {
  struct parameters *tparas = (struct parameters *)paras;
  cout << "No. " << tparas -> tid << " reader thread created." << endl;

  sleep (tparas -> start_time);
  cout << "No. " << tparas -> tid << " reader apply for reading..." << endl;

  sem_wait(&mutex);
  read_count++;
  if (read_count == 1) { sem_wait(&write_file); }
  sem_post(&mutex);

  cout << "No. " << tparas -> tid << " reader start reading..." << endl;
  sleep(tparas -> end_time);
  cout << "No. " << tparas -> tid << " reader end up reading." << endl;

  sem_wait(&mutex);
  read_count--;
  if (read_count == 0) { sem_post(&write_file); }
  sem_post(&mutex);

  free(tparas);
}

void *writer(void *paras) {
  struct parameters *tparas = (struct parameters *)paras;
  cout << "No. " << tparas -> tid << " writer thread created." << endl;

  sleep(tparas -> start_time);
  cout << "No. " << tparas -> tid << " writer apply for writing..." << endl;

  sem_wait(&write_file);
  cout << "No. " << tparas -> tid << " writer start writing..." << endl;
  sleep(tparas -> end_time);
  cout << "No. " << tparas -> tid << " writer end up writing." << endl;
  sem_post(&write_file);

  free(tparas);
}