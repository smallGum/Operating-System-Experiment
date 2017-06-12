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
  int start_time, end_time;
};

int read_count = 0;  // record the number of readers that are reading the file
int write_count = 0; // record the number of writers that are writing the file
sem_t mutex1;        // reader-reader mutex for read_count
sem_t mutex2;
sem_t mutex3;
sem_t write_file;    // critial object for writer-writer mutex and reader-writer mutex
sem_t read_file;
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
  sem_init(&mutex1, 0, 1);
  sem_init(&mutex2, 0, 1);
  sem_init(&mutex3, 0, 1);
  sem_init(&write_file, 0, 1);
  sem_init(&read_file, 0, 1);

  // create threads to read or write the file
  for (int i = 0; i < nthreads; ++i) {
    char role;
    struct parameters *paras = new struct parameters[1];
    fin >> tids[i] >> role >> paras -> start_time >> paras -> end_time;

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
  sem_destroy(&mutex1);
  sem_destroy(&mutex2);
  sem_destroy(&mutex3);
  sem_destroy(&write_file);
  sem_destroy(&read_file);
  // close the file
  fin.close();

  return 0;
}

void *reader(void *paras) {
  cout << "new reader thread created." << endl;
  struct parameters *tparas = (struct parameters *)paras;

  sleep (tparas -> start_time);
  cout << "new reader apply for reading..." << endl;

  sem_wait(&mutex2);
  sem_wait(&mutex1);
  read_count++;
  if (read_count == 1) { sem_wait(&write_file); }
  sem_post(&mutex1);
  sem_post(&mutex2);

  sem_wait(&read_file);
  cout << "reader start reading..." << endl;
  sleep(tparas -> end_time);
  cout << "reader end up reading." << endl;
  sem_post(&read_file);

  sem_wait(&mutex1);
  read_count--;
  if (read_count == 0) { sem_post(&write_file); }
  sem_post(&mutex1);

  free(tparas);
}

void *writer(void *paras) {
  cout << "new writer thread created." << endl;
  struct parameters *tparas = (struct parameters *)paras;

  sleep(tparas -> start_time);
  cout << "new writer apply for writing..." << endl;

  sem_wait(&mutex3);
  write_count++;
  if (write_count == 1) { 
    sem_wait(&mutex2);
  	sem_wait(&read_file); 
  }
  sem_post(&mutex3);

  sem_wait(&write_file);
  cout << "writer start writing..." << endl;
  sleep(tparas -> end_time);
  cout << "writer end up writing." << endl;
  sem_post(&write_file);
  
  sem_wait(&mutex3);
  write_count--;
  if (write_count == 0) { 
  	sem_post(&read_file); 
    sem_post(&mutex2);
  }
  sem_post(&mutex3);

  free(tparas);
}