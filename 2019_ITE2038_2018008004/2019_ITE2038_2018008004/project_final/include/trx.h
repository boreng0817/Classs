#ifndef __TRX_H__
#define __TRX_H__

#include "join.h"
#include <pthread.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
//using namespace std;

enum lock_mode{SHARED, EXCLUSIVE};
enum trx_state{IDLE, RUNNING, WAITING};
enum record_lock_state{SUCCESS, CONFLICT, DEADLOCK};

#define LOCK_LIST_HEAD NULL
#define LOCK_LIST_TAIL NULL
typedef struct lock_list_t {
    struct lock_t *head;
    struct lock_t *tail;
} lock_list_t;

typedef struct undo_log_t {
    int table_id;
    my_key_t key;
    std::string old_value;
} undo_log_t;

typedef std::pair<int,int> tableId_pageId;

typedef struct lock_t {
    int table_id;
    struct trx_t *trx;
    
    int page_id;
    int record_id; // key
    
    
    bool acquired;
    bool tail;
    enum lock_mode mode;

    lock_t *prev;
    lock_t *next;
} lock_t;

typedef struct trx_t {
    int trx_id;

    enum trx_state state;
    

    std::vector<lock_t*> trx_locks;
    
    pthread_mutex_t trx_mutex;
    pthread_cond_t trx_cond;

    lock_t *wait_lock;
    trx_t *trx_wait_for_end;
    std::vector<undo_log_t> undo_log_list;
} trx_t;

struct hash_pair {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
    }
};

typedef struct lock_mgr_t {
    std::unordered_map<tableId_pageId, lock_list_t *, hash_pair> lock_table;
    pthread_mutex_t lock_mgr_mutex = PTHREAD_MUTEX_INITIALIZER;
} lock_mgr_t;

typedef struct trx_mgr_t {
    std::unordered_map<int, trx_t *> trx_table;
    int next_trx_id = 0;
    pthread_mutex_t trx_mgr_mutex = PTHREAD_MUTEX_INITIALIZER;

} trx_mgr_t;

/** global variable **/
extern trx_mgr_t g_trx_mgr;
extern lock_mgr_t g_lock_mgr;

int begin_trx();
int end_trx(int tid);
int db_find(int table_id, int64_t key, char* ret_val, int trx_id);
int db_update(int table_id, int64_t key, char* values, int trx_id);


/* helper function */
inline void acquire_trx_mgr_latch();
inline void release_trx_mgr_latch();
inline void acquire_buf_pool_latch();
inline void release_buf_pool_latch();
inline void acquire_lock_table_latch();
inline void release_lock_table_latch();
int try_acquire_buf_page_latch(int frame_index);
void release_buf_page_latch(int frame_index);
record_lock_state acquire_record_lock(int table_id, 
                                      int key, 
                                      int trx_id,
                                      int page_id,
                                                    lock_mode mode);
lock_t *lock_obj_make(int table_id, int key, int trx_id, int page_id,
                                                            lock_mode mode);
void lock_mgr_append_lock_obj(lock_t *lock, lock_list_t *list);
record_lock_state deadlock_detection(int tid);
void trx_fill_dependency(int tid);
bool trx_cycle_detect(int tid);
void trx_wait_trx(int trx_id);
void lock_table_remove(lock_t *lock);
void erase_lock_obj(lock_t *lock);
lock_t *lock_make_acquired_obj(int table_id, int key, int trx_id, int page_id,
                                                            lock_mode mode);

void add_lock_obj(lock_t *pivot_lock, lock_t *inserted_lock, lock_list_t *list);
lock_list_t *lock_list_init();

lock_t *lock_list_scan(lock_t *lock, lock_list_t *list, int key, int trx_id);
void lock_wake_up(lock_t* lock);
void abort_trx(int tid);
#endif /* __TRX_H__ */
