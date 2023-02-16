#include <gtest/gtest.h>
#include <thread>
#include "NaiveSnapshot.h"
#include "SimpleSnapshot.h"
#include "WFSnapshot.h"
#include <vector>

using namespace std;

ISnapshot *Snapshot;
int **snaps;
atomic<int> stamp = 0;

int thread_creation_count = 0;
int THREAD_COUNT;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread routine.
void thread_function(int tid) {

  int count = 1000;

  // For synchronized start.
  pthread_mutex_lock(&mutex);
  thread_creation_count++;
  pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);

  while (--count) {
    // do update
    Snapshot->update((tid + 1) * count, tid);
    // log snapshot
    snaps[stamp++] = Snapshot->scan();
  }

  return;
}

// snapshot test
TEST(SnapshotTest, WFSnapshot) {
  // number of threads
  int N = 3;
  vector<thread> threads;

  snaps = new int*[(N + 1) * 1000];

  Snapshot = (ISnapshot*)(new WFSnapshot(N));

  thread_creation_count = 0;

  for (int i = 0; i < N; ++i) {
    threads.push_back(thread(thread_function, i));
  }

  // Wait until all threads are created correctly
  while (thread_creation_count < N) {
    sched_yield();
  }

  // Start worker threads simultaneously
  pthread_mutex_lock(&mutex);
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);

  for (int i = 0; i < N; ++i) {
    threads[i].join();
  }

  // Check if snapshot has valid range of number. [0, (tid + 1) * 1000]
  for (int i = 0; i < stamp; ++i) {
    for (int j = 0; j < N; ++j) {
      EXPECT_TRUE(0 <= snaps[i][j] && snaps[i][j] < (j + 1) * 1000);
    }
  }

  // Last snapshot will be {1, 2, 3}
  cout << "\nlast snap shot expected : 1, 2, 3\n";
  cout <<   "result                  : " << snaps[stamp-1][0] << ", " << snaps[stamp-1][1] << ", " << snaps[stamp - 1][2] << "\n\n";
}

