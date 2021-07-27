#include "buf.h"
#include "join.h"
/* global variable for buffer */
buffer g_buf;

/* initiate buffer with size of [buf_num]
 * initialize g_buf
 */
int init_db(int buf_num) {
    g_buf = buf_init(buf_num);
    
    if (g_buf != NULL)
        return 0;
    else
        return -1;
}

/* wrapper function of _open_table(pathname)
 * will try to open table if it exists, 
 * or make a table and open.
 */
int open_table(char *pathname) {
    return _open_table(pathname);
}

/* check if table_num is valid for g_buf.
 * then, call [_db_insert] which is a insert operation
 * for bpt.
 */
int db_insert(int table_num, my_key_t key, char* value) {
    
    if (g_buf -> table_ids[table_num] == BUF_EMPTY_TABLE)
        return -1;

    return _db_insert(table_num, key, value);
}

/* check if table_num is valid for g_buf.
 * then, call [_db_find] which is a find operation
 * for bpt.
 */
int db_find(int table_num, my_key_t key, char* ret_val) {

    if (g_buf -> table_ids[table_num] == BUF_EMPTY_TABLE)                       
        return -1;

    return _db_find(table_num, key, ret_val);
}

/* check if table_num is valid for g_buf.
 * then, call [_db_delete] which is a delete operation
 * for bpt.
 */
int db_delete(int table_num, my_key_t key) {

    if (g_buf -> table_ids[table_num] == BUF_EMPTY_TABLE)                       
        return -1;

    return _db_delete(table_num, key);
}

/* wrapper function of [_close_table].
 * it will check whether table is opened or not.
 * if opened, close table. 
 * if not, return failure signal.
 */
int close_table(int table_id) {
    return _close_table(table_id);
}

/* flush all frame into tables.
 * then, free whole g_buf.
 */
int shutdown_db() {
    
    int i;
    for (i = g_buf -> LRU_head; i != LRU_LIST_TAIL; i = g_buf -> LRU_head) {
        buf_evict_frame(i);
    }
    buf_free();
    
    return 0;
}

int join_table(int table_id_1, int table_id_2, char *pathname) {
    return _join_table(table_id_1, table_id_2, pathname);
}
