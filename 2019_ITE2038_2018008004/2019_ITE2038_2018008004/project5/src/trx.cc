#include "trx.h"
using namespace std;

void print_list(lock_list_t *list) {

    lock_t *iter = list -> head;

    while (iter != LOCK_LIST_TAIL) {
        printf("trx_id[%d]acq[%d] ", iter -> trx -> trx_id, iter -> acquired);
        iter = iter -> next;
    }
    printf("\n");
}

int begin_trx() {

    acquire_trx_mgr_latch();

    trx_t *trx = (trx_t*)malloc(sizeof(trx_t));

    trx -> trx_id = g_trx_mgr.next_trx_id;
    g_trx_mgr.next_trx_id += 1;

    trx -> trx_mutex = PTHREAD_MUTEX_INITIALIZER;
    trx -> trx_cond = PTHREAD_COND_INITIALIZER;

    trx -> state = RUNNING;

    trx -> wait_lock = NULL;
    trx -> trx_wait_for_end = NULL;

    g_trx_mgr.trx_table[trx -> trx_id] = trx;

    release_trx_mgr_latch();

    return trx -> trx_id;
}

int db_find(int table_id, int64_t key, char* ret_val, int trx_id) {

    int frame_index;
    pagenum_t pagenum;
    record_lock_state rl_state;
    
    //0. check table is opened or not

    if (table_id < 0 || table_id > 10 || 
            g_buf -> table_ids[table_id] == BUF_EMPTY_TABLE)
        return -1; //table not found
    if (g_trx_mgr.trx_table[trx_id] == 0)
        return -1; //trx not found
    
    while (1) {

        acquire_buf_pool_latch();

        frame_index = buf_read_page(table_id, 
                find_leaf_page(table_id, key),
                NULL);

        if (try_acquire_buf_page_latch(frame_index) == 0) {
            release_buf_pool_latch();
            continue;
        }

        release_buf_pool_latch();
        pagenum = g_buf->frames[frame_index].page_num;

        rl_state = acquire_record_lock(table_id, 
                key, 
                trx_id, 
                pagenum, 
                SHARED);

        if (rl_state == SUCCESS)
            break;

        else if (rl_state == DEADLOCK) {
            release_buf_page_latch(frame_index);
            abort_trx(trx_id);
            return -1; // FAIL
        }

        else if (rl_state == CONFLICT) {
            release_buf_page_latch(frame_index);
            trx_wait_trx(trx_id);
            continue;
        }

        else {
            cout << "you can't enter here!\n";
        }

    }

    //6. Do find
    frame *frame_ptr = &g_buf->frames[frame_index];
    leaf_page_t *leaf_page = (leaf_page_t*)&(frame_ptr -> page);
    int i;

    if (pagenum == 0) {
        release_buf_page_latch(frame_index);
        return -1; // page not found
    }

    for (i = 0; i < leaf_page -> num_of_keys; ++i)
        if (leaf_page -> records[i].key == key) break;
    
    if (i == leaf_page -> num_of_keys) {
        release_buf_page_latch(frame_index);
        return -1; // record not found
    }
    strcpy(ret_val, leaf_page -> records[i].value);
    release_buf_page_latch(frame_index);

    return 0; // SUCCESS
}

int db_update(int table_id, int64_t key, char* values, int trx_id) {

    int frame_index;
    pagenum_t pagenum;
    record_lock_state rl_state;
    //0. check table is opened or not
    if (table_id < 0 || table_id > 10 || 
            g_buf -> table_ids[table_id] == BUF_EMPTY_TABLE)
        return -1; //table not found

    if (g_trx_mgr.trx_table[trx_id] == 0)
        return -1; //trx not found
 

    while (1) {

        acquire_buf_pool_latch();

        frame_index = buf_read_page(table_id, 
                find_leaf_page(table_id, key),
                NULL);

        if (try_acquire_buf_page_latch(frame_index) == 0) {
            release_buf_pool_latch();
            continue;
        }

        release_buf_pool_latch();
        pagenum = g_buf->frames[frame_index].page_num;

        rl_state = acquire_record_lock(table_id, 
                key, 
                trx_id, 
                pagenum, 
                EXCLUSIVE);

        if (rl_state == SUCCESS)
            break;

        else if (rl_state == DEADLOCK) {
            //5-1) deadlock handling
            release_buf_page_latch(frame_index);
            abort_trx(trx_id);
            return -1; // FAIL
        }

        else if (rl_state == CONFLICT) {
            release_buf_page_latch(frame_index);
            trx_wait_trx(trx_id);
            continue;
        }

        else {
            cout << "you can't enter here!\n";
        }
    }
    //6. Do Update

    frame *frame_ptr = &g_buf->frames[frame_index];
    leaf_page_t *leaf_page = (leaf_page_t*)&(frame_ptr -> page);
    int i;
    undo_log_t undo;
    trx_t *trx = g_trx_mgr.trx_table[trx_id];
    
    if (pagenum == 0) {
        release_buf_page_latch(frame_index);
        return -1;
    }

    for (i = 0; i < leaf_page -> num_of_keys; ++i)
        if (leaf_page -> records[i].key == key) break;
    
    if (i == leaf_page -> num_of_keys) {
        release_buf_page_latch(frame_index);
        return -1; // record not found
    }
    string str(leaf_page -> records[i].value);
    undo.old_value = str;
    undo.table_id = table_id;
    undo.key = key;

    strcpy(leaf_page -> records[i].value, values);
    frame_ptr -> is_dirty = 1;
    trx -> undo_log_list.push_back(undo);
    release_buf_page_latch(frame_index);

    return 0; // SUCCESS

}

int end_trx(int tid) {

    trx_t *trx = g_trx_mgr.trx_table[tid];

    if (trx == 0) 
        return 0; // trx [tid] is not in trx_table

    // 1. Acquire the lock table latch

    acquire_lock_table_latch();
    // 2. release locks and wake up
    while (trx -> trx_locks.size()) {
        lock_table_remove(trx->trx_locks[trx -> trx_locks.size() - 1]);
        trx->trx_locks.pop_back();
    }
    pthread_mutex_lock(&(trx->trx_mutex));
    pthread_cond_broadcast(&(trx->trx_cond));
    pthread_mutex_unlock(&(trx->trx_mutex));
    // 3. release lock table latch
    release_lock_table_latch();

    acquire_trx_mgr_latch();

    trx -> state = IDLE;
    release_trx_mgr_latch();

    return tid;
}

void abort_trx(int tid) {
    

    trx_t *trx = g_trx_mgr.trx_table[tid];
    int frame_index;
    undo_log_t undo;

    if (trx -> undo_log_list.size() != 0)
        undo = trx -> undo_log_list[trx -> undo_log_list.size() - 1];
    
    while ( trx -> undo_log_list.size() ) {

        acquire_buf_pool_latch();

        frame_index = buf_read_page(undo.table_id,
                find_leaf_page(undo.table_id, undo.key),
                NULL);

        if (try_acquire_buf_page_latch(frame_index) == 0) {
            release_buf_pool_latch();
            continue;
        }

        release_buf_pool_latch();

        frame *frame_ptr = &g_buf -> frames[frame_index];
        leaf_page_t *leaf_page = (leaf_page_t*)&(frame_ptr -> page);
        int i;

        for (i = 0; i < leaf_page -> num_of_keys; ++i)
            if (leaf_page -> records[i].key == undo.key) break;

        strcpy(leaf_page -> records[i].value, undo.old_value.c_str());
        frame_ptr -> is_dirty = 1;

        release_buf_page_latch(frame_index);

        trx -> undo_log_list.pop_back();
        undo = trx -> undo_log_list[trx -> undo_log_list.size() - 1];
    }
    end_trx(tid);
}

inline void acquire_trx_mgr_latch() {
    pthread_mutex_lock(&(g_trx_mgr.trx_mgr_mutex));
}

inline void release_trx_mgr_latch() {
    pthread_mutex_unlock(&(g_trx_mgr.trx_mgr_mutex));
}

inline void acquire_buf_pool_latch() {
    pthread_mutex_lock(&(g_buf -> buf_pool_mutex));
}

inline void release_buf_pool_latch() {
    pthread_mutex_unlock(&(g_buf -> buf_pool_mutex));
}

inline void acquire_lock_table_latch() {
    pthread_mutex_lock(&(g_lock_mgr.lock_mgr_mutex));
}

inline void release_lock_table_latch() {
    pthread_mutex_unlock(&(g_lock_mgr.lock_mgr_mutex));

}

int try_acquire_buf_page_latch(int frame_index) {

    pthread_mutex_trylock(&g_buf->frames[frame_index].buf_page_mutex);

    if (g_buf -> frames[frame_index].acquired)
        return 0; //failed to get lock
    else {
        g_buf -> frames[frame_index].acquired = true;
        return 1;
    }
}

void release_buf_page_latch(int frame_index) {

    g_buf -> frames[frame_index].acquired = false;
    pthread_mutex_unlock(&g_buf->frames[frame_index].buf_page_mutex);

}

record_lock_state acquire_record_lock(int table_id,
        int key,
        int trx_id,
        int page_id,
        lock_mode mode) {

    lock_t *new_lock, *iter;
    lock_list_t *lock_list;
    record_lock_state mode_to_return;
    acquire_lock_table_latch();

    lock_list = g_lock_mgr.lock_table[make_pair(table_id, page_id)];

    if (lock_list == 0) {
        lock_list = lock_list_init();
        g_lock_mgr.lock_table[make_pair(table_id, page_id)] = lock_list;
    }

    /* scan list */
    iter = lock_list_scan(iter, lock_list, key, trx_id);
    /* if found */
    if (iter != LOCK_LIST_TAIL) {

        if (iter -> acquired && iter -> trx -> trx_id == trx_id) {

            /* lock mode promotion */
            if (iter -> mode == SHARED && mode == EXCLUSIVE) {
                iter -> mode = EXCLUSIVE;
            }
            release_lock_table_latch();
            return SUCCESS;
        }

        else if (iter -> acquired && iter -> trx -> trx_id != trx_id) {

            if (iter -> mode == SHARED && mode == SHARED) {
                mode_to_return = SUCCESS;
                new_lock = lock_make_acquired_obj(table_id, key, trx_id, 
                        page_id, mode);
                add_lock_obj(iter, new_lock, lock_list);
            }
            else {
                mode_to_return = CONFLICT;
            }
        }

        else if (!iter -> acquired && iter -> trx -> trx_id != trx_id) {
            mode_to_return = CONFLICT;
        }

        else {
            return CONFLICT;
        }
    }
    /* if not found */
    else {
        new_lock = lock_make_acquired_obj(table_id, key, trx_id, page_id, mode);
        lock_mgr_append_lock_obj(new_lock, lock_list);
        mode_to_return = SUCCESS;
    }

    /* there's no lock obj for record[key] in lock table.
     * just append and return SUCCESS!
     */
    if (mode_to_return == CONFLICT) {
        new_lock = lock_obj_make(table_id, key, trx_id, page_id, mode);
        (new_lock -> trx -> trx_locks).push_back(new_lock);
        add_lock_obj(iter, new_lock, lock_list);
        mode_to_return = deadlock_detection(trx_id);
    }
    else
        (new_lock -> trx -> trx_locks).push_back(new_lock);

    release_lock_table_latch();
    return mode_to_return;
}

lock_list_t *lock_list_init() {

    lock_list_t *list = (lock_list_t*)malloc(sizeof(lock_list_t));
    list -> head = LOCK_LIST_HEAD;
    list -> tail = LOCK_LIST_TAIL;
}

lock_t *lock_make_acquired_obj(int table_id, int key, int trx_id, int page_id,
        lock_mode mode) {
    lock_t *lock = lock_obj_make(table_id, key, trx_id, page_id, mode);
    lock -> acquired = true;
    return lock;
}

lock_t *lock_obj_make(int table_id, int key, int trx_id, int page_id, 
        lock_mode mode) {
    lock_t *lock = (lock_t*)malloc(sizeof(lock_t));
    lock -> table_id = table_id;
    lock -> trx = g_trx_mgr.trx_table[trx_id];

    lock -> page_id = page_id;
    lock -> record_id = key;
    lock -> acquired = false;
    lock -> tail = false;
    lock -> mode = mode;

    return lock;
}

void lock_mgr_append_lock_obj(lock_t *lock, lock_list_t *lock_list) {

    /* initial append
    */
    if (lock_list -> head == LOCK_LIST_HEAD) {

        lock_list -> head = lock;
        lock_list -> tail = lock;
        lock -> prev = LOCK_LIST_HEAD;
        lock -> next = LOCK_LIST_TAIL;

    } else {

        lock_t *temp = lock_list -> tail;

        temp -> next = lock;
        lock -> prev = temp;
        lock -> next = LOCK_LIST_TAIL;
        lock_list -> tail = lock;

        lock -> prev -> tail = false;
    }

    lock -> tail = true;

}

record_lock_state deadlock_detection(int tid) {
    /* fill contents [trx -> wait lock] [trx -> trx_wait_for_end]
     * then, check for the cycle.
     * need trx_mgr to detect cycle.
     * need to release lock tbl latch
     */

    bool cycle_detected = false;
    trx_t *trx = g_trx_mgr.trx_table[tid];

    trx -> wait_lock = trx -> trx_locks[trx -> trx_locks.size() -1];
    trx_fill_dependency(tid);

    cycle_detected = trx_cycle_detect(tid);

    if (cycle_detected) 
        return DEADLOCK;
    else
        return CONFLICT;
}

void trx_fill_dependency(int tid) {

    trx_t *trx = g_trx_mgr.trx_table[tid];
    lock_t *iter = trx -> wait_lock;
    lock_mode mode = iter -> mode;

    if (iter != NULL && mode == EXCLUSIVE) {

        iter = iter -> prev;

    } else {

        while (iter != NULL && mode == SHARED) {
            iter = iter -> prev;
            mode = iter -> mode;
        }
    }

    if (iter == NULL)
        return;
    else
        trx -> trx_wait_for_end = iter -> trx;
}

bool trx_cycle_detect(int tid) {

    trx_t *trx = g_trx_mgr.trx_table[tid];
    trx_t *iter = trx -> trx_wait_for_end;

    while (iter != NULL && iter != trx)
        iter = iter -> trx_wait_for_end;

    if (iter == trx)
        return true;

    else
        return false;
}

void trx_wait_trx(int trx_id) {

    trx_t *trx_wait = g_trx_mgr.trx_table[trx_id];
    trx_t *trx_running = trx_wait -> trx_wait_for_end;

    if (trx_wait -> trx_wait_for_end == NULL)
        return;

    pthread_mutex_lock(&(trx_running -> trx_mutex));
    trx_wait -> state = WAITING;

    pthread_cond_wait(&(trx_running -> trx_cond), &(trx_running -> trx_mutex));
    pthread_mutex_unlock(&(trx_running -> trx_mutex));

}


void lock_wake_up(lock_t* wake_up_lock) {
    if (wake_up_lock == NULL)
        return;
    wake_up_lock -> acquired = true;
    wake_up_lock -> trx -> trx_wait_for_end = NULL;
    wake_up_lock -> trx -> state = RUNNING;
}

void lock_table_remove(lock_t *lock) {

    if (!lock -> tail) {
        lock_t *wake_up_lock = lock -> next;

        lock_wake_up(wake_up_lock);

        while (wake_up_lock != NULL && wake_up_lock -> mode != EXCLUSIVE) {
            wake_up_lock = wake_up_lock -> next;
            if (wake_up_lock -> mode == EXCLUSIVE)
                break;
            lock_wake_up(wake_up_lock);
        }
    }

    erase_lock_obj(lock);
}

void add_lock_obj(lock_t *pivot_lock, lock_t *inserted_lock, lock_list_t *list) {

    if (pivot_lock -> next == LOCK_LIST_TAIL) {
        list -> tail = inserted_lock;
        pivot_lock -> next = inserted_lock;

        inserted_lock -> prev = pivot_lock;
        inserted_lock -> next = LOCK_LIST_TAIL;
    }
    else {
        inserted_lock -> next = pivot_lock -> next;
        inserted_lock -> next -> prev = inserted_lock;

        pivot_lock -> next = inserted_lock;
        inserted_lock -> prev = pivot_lock;
    }
    pivot_lock -> tail = false;
    inserted_lock -> tail = true;
}

lock_t *lock_list_scan(lock_t *lock, lock_list_t *list, int key, int trx_id) {


    bool record_lock_found = false;
    bool trx_found = false;
    lock = list -> head;
    lock_t *trx_lock = NULL;

    while (lock != NULL) {
        if (lock -> record_id == key) {
            record_lock_found = true;
            break;
        }
        lock = lock -> next;
    }

    if (record_lock_found) 
        while (!lock -> tail) {
            if (lock -> trx -> trx_id == trx_id && lock -> acquired) {
                trx_found = true;
                trx_lock = lock;
            }
            lock = lock -> next;
        }

    if (record_lock_found && lock -> trx -> state == RUNNING && 
            trx_found && lock -> trx -> wait_lock == trx_lock) {
        lock -> trx -> wait_lock = NULL;
        return trx_lock;
    }

    return lock;
}

void erase_lock_obj(lock_t *lock) {

    pair<int, int> key = make_pair(lock -> table_id, lock ->page_id);
    lock_list_t *list = g_lock_mgr.lock_table[key];

    if (lock -> prev == LOCK_LIST_HEAD && lock -> next == LOCK_LIST_TAIL) {
        list -> head = LOCK_LIST_HEAD;
        list -> tail = LOCK_LIST_TAIL;
    }

    /* first element, #entry > 1
    */
    else if (lock -> prev == LOCK_LIST_HEAD && lock -> next != LOCK_LIST_TAIL) {
        list -> head = lock -> next;
        list -> head -> prev = LOCK_LIST_HEAD;
    }

    else if (lock -> prev != LOCK_LIST_HEAD && lock -> next == LOCK_LIST_TAIL) {
        list -> tail = lock -> prev;
        list -> tail -> next = LOCK_LIST_TAIL;
    }

    else {
        lock -> next -> prev = lock -> prev;
        lock -> prev -> next = lock -> next;
    }

    if (lock -> tail && lock -> prev != NULL)
        lock -> prev -> tail = true;

    free(lock);
}
