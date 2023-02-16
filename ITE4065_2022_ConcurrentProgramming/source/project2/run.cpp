#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "NaiveSnapshot.h"
#include "SimpleSnapshot.h"
#include "WFSnapshot.h"

using namespace std;

// For synchronized start
bool time_signal = false;
int thread_creation_count = 0;
int THREAD_COUNT;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Constant value for program
const chrono::milliseconds SLEEP_DURATION = 60000ms;
char CONFIG_PATH[] = "config.txt";

// result of update count
long long result = 0;

// Interface for NaiveSnapshot, SimpleSnapshot, WFSnapshot.
ISnapshot *Snapshot;

// Thread function that randomly update it's thread until the got finish signal.
void thread_function(int tid) {

  // For synchronized start.
  pthread_mutex_lock(&mutex);
  thread_creation_count++;
  pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);

  long long count = 0;

  while (!time_signal) {
    // do update
    Snapshot->update(rand(), tid);
    count++;
  }

  // update total count mutual exclusively.
  pthread_mutex_lock(&mutex);
  result += count;
  pthread_mutex_unlock(&mutex);

  return;
}

// print usage of this program.
void info(string program) {
  string information =\
           "Give " + program + " an integer argument. (1, 2, 4, 8, 16, 32)\n" +\
           "Ex) " + program + " 8\n";
  cout << information;
}

// parse config.txt for algorithm selection.
int parseConfig(char *path) {
  FILE *f = fopen(path, "r");
  int ret;

  if (fscanf(f, "%d", &ret) == 0) {
    ret = 0;
    cout << "check config.txt file.\n";
  }

  fclose(f);

  return ret;
}


int main(int argc, char *argv[]) {

  // check if there's an integer argument.
  if (argc < 2) {
    info(argv[0]);
    return 0;
  }

  THREAD_COUNT = atoi(argv[1]);
  vector<thread> t;

  // Parse algorithm selection
  int option = parseConfig(CONFIG_PATH);

  srand(time(NULL));

  // Naive snapshot
  if (option == 1) {
    Snapshot = (ISnapshot*)(new NaiveSnapshot(THREAD_COUNT));
    cout << "Naive snapshot\n";
  // SimpleSnapshot
  } else if (option == 2) {
    Snapshot = (ISnapshot*)(new SimpleSnapshot(THREAD_COUNT));
    cout << "Simple snapshot\n";
  // WFSnapshot
  } else if (option == 3) {
    Snapshot = (ISnapshot*)(new WFSnapshot(THREAD_COUNT));
    cout << "Wait free snapshot\n";
  } else {
  // Wrong configuration option.
    cout << "Check [config.txt] file\n";
    return 0;
  }


  // create threads
  for (int i = 0; i < THREAD_COUNT; ++i) {
    t.push_back(thread(thread_function, i));
  }

  // Wait until all threads are created correctly
  while (thread_creation_count < THREAD_COUNT) {
    sched_yield();
  }

  // Start worker threads simultaneously
  pthread_mutex_lock(&mutex);
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);

  // 1 minute timer
  this_thread::sleep_for(SLEEP_DURATION);
  time_signal = true;

  // Wait until all worker threads are finished
  for (int i = 0; i < THREAD_COUNT; ++i) {
    t[i].join();
  }

  // Print result.
  cout << "result : " << result << endl;

  // Clean up some resources
  delete Snapshot;

  return 0;
}
