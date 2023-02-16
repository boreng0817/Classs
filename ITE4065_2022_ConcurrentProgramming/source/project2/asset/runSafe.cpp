#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

bool time_signal = false;
vector<int> local_integer;
int thread_creation_count = 0;
int THREAD_COUNT;
long long result = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void thread_function(int tid) {

    pthread_mutex_lock(&mutex);
    thread_creation_count++;
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    int collect[THREAD_COUNT];
    long long count = 0;

    while (!time_signal) {
        // do scan
        for (int i = 0; i < THREAD_COUNT; ++i) {
            collect[i] = local_integer[i];
        }
        // do update
        local_integer[tid] = rand();

        count++;
    }

    pthread_mutex_lock(&mutex);
    result += count;
    pthread_mutex_unlock(&mutex);

    return;
}

void info(string program) {
    string information =\
            "Give " + program + " integer argument. (1, 2, 4, 8, 16, 32)\n" +\
            "Ex) " + program + " 8\n";
    cout << information;
}


int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        info(argv[0]);
        return 0;
    }

    THREAD_COUNT = atoi(argv[1]);
    vector<thread> t;

    srand(time(NULL));

    for (int i = 0; i < THREAD_COUNT; ++i) {
        local_integer.push_back(0);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        t.push_back(thread(thread_function, i));
    }

    while (thread_creation_count < THREAD_COUNT) {
        pthread_yield();
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    // 1 minute timer
    this_thread::sleep_for(1000ms);
    time_signal = true;

    for (int i = 0; i < THREAD_COUNT; ++i) {
        t[i].join();
    }

    cout << "result : " << result << endl;

    return 0;
}
