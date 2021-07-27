#ifndef __BPT_H__
#define __BPT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> // pread(), pwrite()
#include <string.h>
#include <stdint.h>
#include <fcntl.h> // constants
#include <sys/stat.h> // open()

#define PAGE_SIZE 4096
#define LEAF_ORDER 31
#define INTERNAL_ORDER 248 //branching factor : 249
#define INITIAL_PAGE_NUM 1024
#define VALUE_SIZE 120

#define CAST_TO_PAGE(X) (page_t*)&(X)

typedef uint64_t pagenum_t;
typedef int64_t my_key_t;

typedef struct record {
    my_key_t key;
	char value[VALUE_SIZE];
} record;

typedef struct internal_record {
	my_key_t key;
	pagenum_t page_num;
} internal_record;

typedef struct page_t {
	char bytes[PAGE_SIZE];
} page_t;

typedef struct header_page_t {
    pagenum_t free_page_num;
    pagenum_t root_page_num;
    pagenum_t num_of_pages;

    char reserved[PAGE_SIZE - 24];
} header_page_t;

typedef struct free_page_t {
	pagenum_t next_free_page_num;
	
	char not_used[PAGE_SIZE - 8];
} free_page_t;

typedef struct leaf_page_t {
	pagenum_t parent;
	int is_leaf;
	int num_of_keys;
	char reserved[128 - 24];
	pagenum_t right_sibling;
	
	record records[LEAF_ORDER];
} leaf_page_t;

typedef struct internal_page_t {
	pagenum_t parent;
	int is_leaf;
	int num_of_keys;
	char reserved[128 - 24];
	pagenum_t page_0;
	
	internal_record records[INTERNAL_ORDER];
} internal_page_t;

extern int g_fd;
extern header_page_t g_head;
extern int DELAYED_MERGE;

//File manager API
void file_read_page(int table_num, pagenum_t pagenum, page_t *dest);
void file_write_page(int table_num, pagenum_t pagenum, const page_t *src);
//=====

/*target API*/

//open
int _open_table(char *pathname);
int initiate_table(char *filename);
int _close_table(int table_id);
int simple_hash(char* filename);
//insert
int _db_insert(int table_num, my_key_t key, char* value);
int create_root(int table_num, my_key_t key, char* value);
int insert_into_leaf(int table_num, pagenum_t pagenum, my_key_t key, char* value);
int insert_into_leaf_after_splitting(int table_num, pagenum_t pagenum, my_key_t key, 
                                     char *value);
int insert_into_parent(int table_num, pagenum_t left, my_key_t key, 
                       pagenum_t value, pagenum_t right);
int insert_into_new_root(int table_num, pagenum_t left, my_key_t key, pagenum_t right);
int insert_into_node(int table_num, pagenum_t internal_num, int left_index,                                         my_key_t key, pagenum_t right);
int insert_into_node_after_splitting(int table_num, pagenum_t old_num, int left_index,                                              my_key_t key, pagenum_t right);


int get_left_index(int table_num, pagenum_t parent, pagenum_t left);
int cut(int length);
void insert_record_leaf(leaf_page_t *leaf, my_key_t key, char *value, int i);
void insert_record_internal(internal_page_t *internal, 
                            my_key_t key, pagenum_t value, int i);

//find
pagenum_t find_leaf_page(int table_num, my_key_t key);
int _db_find(int table_num, my_key_t key, char* ret_val); 

//delete
int get_neigbor_index(int table_num, pagenum_t page_num);
void remove_entry_from_node(int table_num, pagenum_t page_num, my_key_t key);
void adjust_root(int table_num);
void coalesce_nodes(int table_num, pagenum_t node_page_num, 
                    pagenum_t neighbor_page_num, int neighbor_index, 
                    int k_prime);
void redistribute_node(int table_num, pagenum_t node_page_num, 
                       pagenum_t neighbor_page_num, int neighbor_index, 
                       int k_prime_index, int k_prime);     
void delete_entry(int table_num, pagenum_t node_page_num, my_key_t key);
int _db_delete(int table_num, my_key_t key);
int toggle_delayed_merge();
/*util*/

typedef struct queue{                                                                
    pagenum_t page;                                                             
    struct queue *next;                                                                
} queue;
queue *q;

int path_to_root(int table_num, pagenum_t page_num);
void enqueue(pagenum_t n);
pagenum_t dequeue();
void print_tree(int table_num, int do_print);
void usage_1();
void usage_2();
#endif /* __BPT_H__*/
