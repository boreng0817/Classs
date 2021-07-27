#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "bpt.h"

// =====================
#define HASH_EMPTY -10
#define HASH_DEL -1
#define FRAME_NOT_FOUND -1
typedef struct Hash *hash;
typedef struct Hash {
    int capacity;
    int num_element;
    int *arr;
} Hash;

hash hash_init(int capa);
int hash_get_and_set_table_size(int size);
int hash_table_page(int table, pagenum_t page);
void hash_insert(hash H, int hash_digest, int frame_num);
void hash_delete(hash H, int hash_digest, int frame_num);
int hash_get_frame(hash H, int table, pagenum_t page); // fail when -1, success when right frame
void hash_free(hash H); 
// ====================
#define STACK_IS_EMPTY(H) (H)->num_element == 0
#define STACK_IS_FULL(H) (H)->num_element == (H)->capacity
typedef struct Stack *stack;
typedef struct Stack {
    int capacity;
    int num_element;
    int *arr;
} Stack;

stack stack_init(int capa);
void stack_push(stack S, int frame);
int stack_pop(stack S);
void stack_free(stack S);
// =======================
/*
typedef struct List *list;
typedef struct List {
    list *next;
    list *prev;
    int frame_num;
} List;

list list_init();
list list_pop(list L);
list list_delete(list L, );
void list_append(list tail, list element);
*/

typedef int tablenum_t;

#define SIZE_OF_FRAME sizeof(frame)
typedef struct frame {
    page_t page;
    pagenum_t page_num;
    int table_id;
    short is_dirty;
    short is_pinned;
    int next;
    int prev;
} frame;


void frame_delete(int frame_index);
void frame_append(int frame_index);
int frame_pop();

#define LRU_LIST_HEAD -1
#define LRU_LIST_TAIL -2
#define EVICT_SIGNAL -10
#define MAX_TABLE_SIZE 10
#define BUF_EMPTY_TABLE -1
#define BUF_GET_FRAME(buf, index) (buf) -> frames[(index)]
#define BUF_GET_PAGE_NUM(buf, index) (buf) -> frames[(index)].page_num
#define BUF_GET_TABLE_ID(buf, index) (buf) -> frames[(index)].table_id
typedef struct Buffer *buffer;
typedef struct Buffer {
    int capacity;
    int num_used_frame;
    frame *frames;

    int *fds;
    int *table_ids;
    int table_num;

    stack empty_frame;
    int LRU_head;
    int LRU_tail;
    hash frame_in_usage;
    stack *table_buffer;
} Buffer;

buffer buf_init(int size);
pagenum_t buf_alloc_page(tablenum_t table_num);
void buf_free_page(tablenum_t table_num, pagenum_t page_num);
void buf_expand_file(tablenum_t table_num);
int buf_read_page(tablenum_t table_num, pagenum_t page_num, page_t *dest);
int buf_write_page(tablenum_t table_num, pagenum_t page_num, page_t *src);
void buf_evict_frame(int index);
void buf_set_pin(int frame_num);
void buf_unset_pin(int frame_num);
void buf_free();

/* global */
extern buffer g_buf;
extern int g_table_size;

/* api */
int init_db(int buf_num);
int open_table(char *pathname);
int db_insert(int table_num, my_key_t key, char* value); 
int db_find(int table_num, my_key_t key, char* ret_val);
int db_delete(int table_num, my_key_t key);
int close_table(int table_id);
int shutdown_db();
#endif /* __BUFFER_H__ */
