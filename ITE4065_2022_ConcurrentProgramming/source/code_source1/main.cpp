#include <iostream>
#include <thread>

#include <future>
#include <deque>

#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include "Joiner.hpp"
#include "Parser.hpp"

#define THREAD_COUNT 40
#define MIN(a, b) (a) < (b) ? (a) : (b)

using namespace std;

// Mutex and conditional variable for thread pool
// for saving threads, use master cond/mutex to wake
// master thread when all slave threads finish their work.
// Also when slave threads are successfully created.
pthread_mutex_t mutex_slave = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_slave = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_master = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_master = PTHREAD_MUTEX_INITIALIZER;

// For online-batch input, use job_done flag.
// When all batches are fully computed and 
// program is ready to finish, master thread set flag on.
bool job_done = false;
// Use counter for slave thread notify master thread
// that slave threads are successfully created.
int thread_creation_count = 0;
int thread_query_done_count;
// remained query for thread.
int thread_work_todo;

// gather thread result
string thread_ret[THREAD_COUNT];

// deque for storing thread's work 
deque<QueryInfo> queries;


//---------------------------------------------------------------------------
// thread function for slave threads.
// It bascially perform each query, and store their result into corresponding
// index of [thread_ret].
void thread_join(int tid, shared_ptr<Joiner> joiner) {

    // section for equi-start for every slave threads.
    pthread_mutex_lock(&mutex_slave);
    // thread_creation_count is required to recognize every threads are
    // ready to start.
    thread_creation_count++;
    // when thread_creation_count reaches THREAD_COUNT, it means every threads
    // are ready to go. So inform it to master thread.
    if (thread_creation_count == THREAD_COUNT) {
        pthread_mutex_lock(&mutex_master);
        pthread_cond_signal(&cond_master);
        pthread_mutex_unlock(&mutex_master);
    }
    pthread_cond_wait(&cond_slave, &mutex_slave);
    pthread_mutex_unlock(&mutex_slave);

    // do work
    while (!job_done) {
        // if there's more thread than the number of queries in batch,
        // some thread shouldn't perform it's work (since queries's size is
        // less than thread_work_todo.
        if (tid < thread_work_todo)
            thread_ret[tid] = joiner->join(queries[tid]);

        // check this thread did perform query.
        pthread_mutex_lock(&mutex_slave);
        thread_query_done_count++;

        // when thread_query_done_count reaches THREAD_COUNT,
        // it's time to imform master thread to produce more
        // queries.
        if (thread_query_done_count == THREAD_COUNT) {
            pthread_mutex_lock(&mutex_master);
            pthread_cond_signal(&cond_master);
            pthread_mutex_unlock(&mutex_master);
        }

        pthread_cond_wait(&cond_slave, &mutex_slave);
        pthread_mutex_unlock(&mutex_slave);
    }

    return;
}

//---------------------------------------------------------------------------
// previous intra-query parallelization.
// use promise and future for getting value.
void join_wrapper(QueryInfo query, shared_ptr<Joiner> joiner, promise<string> p) {
    string ret = joiner->join(query);

    p.set_value(ret);
}

//---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    auto joiner = new Joiner();

    // shared pointer for Joiner. It used for thread function.
    shared_ptr<Joiner> joiner_ptr(joiner);
    // thread pool
    thread t[THREAD_COUNT];

    // Thread creation.
    for (int i = 0; i < THREAD_COUNT; ++i) {
        t[i] = thread(thread_join, i, joiner_ptr);
    }

    // wait until every slave threads are ready.
    pthread_mutex_lock(&mutex_master);
    pthread_cond_wait(&cond_master, &mutex_master);
    pthread_mutex_unlock(&mutex_master);



    // Read join relations
    // when Driver got relation path, it makes meta data
    // for latter query re-writer.
    string line;
    while (getline(cin, line)) {
        if (line == "Done") break;
        joiner_ptr->addRelation(line.c_str());
    }

    // Add relation metadata to QueryInfo, which works as a
    // query parser.
    QueryInfo info;
    for (auto &relation : joiner_ptr->relations) {
        info.profiles.push_back(relation.profiles);
    }

    while (getline(cin, line)) {
        // Execute per batch
        if (line != "F")  {
            // It rewrites original query based on analyze.
            // Simply, it reorganize predicates (= join) sequence
            // that has least esimated calculate cost.
            info.analyzeQuery(line);
            queries.push_back(info);
        }

        else {
            while (true) {
                thread_work_todo = MIN(queries.size(), THREAD_COUNT);
                // when queries queue is empty, end this batch.
                if (thread_work_todo == 0) {
                    break;
                }

                // reset thread_query_done_count for slave threads.
                thread_query_done_count = 0;

                // wake slave threads to start their job.
                pthread_mutex_lock(&mutex_slave);
                pthread_mutex_lock(&mutex_master);
                pthread_cond_broadcast(&cond_slave);
                pthread_mutex_unlock(&mutex_slave);


                // wait until slave threads finish their job.
                pthread_cond_wait(&cond_master, &mutex_master);
                pthread_mutex_unlock(&mutex_master);


                // print result of performed query, and remove queries
                // that slave thread had finished.
                for (int i = 0; i < thread_work_todo; ++i) {
                    cout << thread_ret[i];
                    queries.pop_front();
                }
            }
            // clear queries
            queries.clear();
        }
    }

    // set flag job_done, wake all threads
    job_done = true;
    pthread_mutex_lock(&mutex_slave);
    pthread_cond_broadcast(&cond_slave);
    pthread_mutex_unlock(&mutex_slave);



    // clean up threads.
    for (int i = 0; i < THREAD_COUNT; ++i) {
        t[i].join();
    }


    return 0;
}
