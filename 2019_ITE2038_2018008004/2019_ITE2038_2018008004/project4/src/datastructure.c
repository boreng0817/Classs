#include "buf.h"

/* global variable */
buffer g_buf;
int g_table_size;

/* initiating hash */
hash hash_init(int capa) {
    
    int i;
    hash H = (hash)malloc(sizeof(Hash));
    H -> capacity = hash_get_and_set_table_size(capa * 10);
    H -> num_element = 0;
    H -> arr = (int*)malloc(sizeof(int) * (H -> capacity));
    
    for (i = 0; i < H -> capacity ; ++i) {
        H -> arr[i] = HASH_EMPTY;
    }

    return H;
}

/* Find a prime number that is larger than [size]
 * in expectation, under 100,000, there  are nearly 10% of prime numbers
 * so the loop will executed (size/2) * 10 in worst case.
 */
int hash_get_and_set_table_size(int size) {

    int table_size = size, i;
    
    while (1) {
        for (i = 2; i <= table_size/2 && table_size % i != 0; ++i);

        if (i == table_size/2 + 1)
            break;
        else
            table_size++;
    }
    g_table_size = table_size;
    
    return g_table_size;

}

/* hash function for table, page.
 * need to be improved in further project.
 */
int hash_table_page(int table, pagenum_t page) {
    int ret = 0;
    int prime0 = 1000003,
        prime1 = 8971,
        prime2 = 13001,
        prime3 = 897829;
    ret = ( ( page % 100 ) * prime1 )   % prime0;
    ret = ( ret + (page / 100)*prime2 ) % prime0;
    ret = ( ret + table * prime3 )      % prime0;

    return ret % g_table_size;
}

/* insert frame into hash table.
 */
/* uses linear probing */
void hash_insert(hash H, int hash_digest, int frame_num) {

    int i, val;
    if(H -> arr[hash_digest] == HASH_EMPTY || 
            H -> arr[hash_digest] == HASH_DEL)
        H -> arr[hash_digest] = frame_num;
    else {
        val = H -> arr[(hash_digest + 1) % g_table_size];
        
        for (i = 1; val != HASH_EMPTY && val != HASH_DEL ; ++i)
            val = H -> arr[(hash_digest + i + 1) % g_table_size];
        H -> arr[(hash_digest + i) % g_table_size] = frame_num;
    }

    H -> num_element += 1;
}

/* find corresponding frame, and delete from hash table.
 */
void hash_delete(hash H, int hash_digest, int frame_num) {

    int i, val;
    if(H -> arr[hash_digest] == frame_num)
        H -> arr[hash_digest] = HASH_DEL;
    else {
        val = H -> arr[(hash_digest + 1) % g_table_size];
        for (i = 1; val != frame_num; ++i) {
            val = H -> arr[(hash_digest + i + 1) % g_table_size];
            if (val == HASH_EMPTY)
                return;
        }

        H -> arr[(hash_digest + i) % g_table_size] = HASH_DEL;                                  
    }

    H -> num_element -= 1;
}

/* checking function wheter [table][page] is exist in frame.
 * fail when -1, success when right frame
 */
int hash_get_frame(hash H, int table, pagenum_t page) { 

    int i, frame_num; 
    int hash_digest = hash_table_page(table, page);
    
    frame_num = H -> arr[hash_digest];
    i = 1;
    for (i = 1; i < g_table_size && frame_num != HASH_EMPTY; ++i) {
        if (frame_num != HASH_DEL && 
            BUF_GET_PAGE_NUM(g_buf, frame_num) == page &&
            BUF_GET_TABLE_ID(g_buf, frame_num) == table)
            return frame_num;
        frame_num = H -> arr[(hash_digest + i)%g_table_size];
    }

    return FRAME_NOT_FOUND;
}

/* free hash structure.
 */
void hash_free(hash H) {
    
    free(H -> arr);
    free(H);
}

/* initiating function for stack.
 */
stack stack_init(int capa) {
    
    stack S = (stack)malloc(sizeof(Stack));
    S -> capacity = capa;
    S -> num_element = 0;
    S -> arr = (int*)malloc(sizeof(int) * capa);

    return S;
}

/* push usable frame into stack.
 */
void stack_push(stack S, int frame) {

    if (S -> capacity == S -> num_element)
        printf("Error occured at stack_push.\n"
               "Can't exceed stack's max capacity.\n");
    S -> arr[(S->num_element)++] = frame;
}

/* pop frame to use.
 */
int stack_pop(stack S) {
    
    if (!(S -> num_element))
        return EVICT_SIGNAL;
    else
        return S -> arr[--(S->num_element)];
}

/* free function for stack
 */
void stack_free(stack S) {

    free(S -> arr);
    free(S);
}

/* maintaining LRU list.
 * delete frame from LRU list.
 */
void frame_delete(int frame_index) {

    /* |Case| element is first element in the list */
    if (BUF_GET_FRAME(g_buf, frame_index).prev == LRU_LIST_HEAD)
        frame_pop();

    /* |Case| element is last element in the list */
    else if(BUF_GET_FRAME(g_buf, frame_index).next == LRU_LIST_TAIL) {
        g_buf -> LRU_tail = BUF_GET_FRAME(g_buf, frame_index).prev;
        BUF_GET_FRAME(g_buf, g_buf -> LRU_tail).next = LRU_LIST_TAIL;
    }
    
    /* |Case| element is in the middle of list */
    else {
        int next, prev;
        next = BUF_GET_FRAME(g_buf, frame_index).next;
        prev = BUF_GET_FRAME(g_buf, frame_index).prev;

        BUF_GET_FRAME(g_buf, prev).next = next;
        BUF_GET_FRAME(g_buf, next).prev = prev;
    }
    hash_delete(g_buf -> frame_in_usage, 
            hash_table_page(BUF_GET_TABLE_ID(g_buf, frame_index), 
                            BUF_GET_PAGE_NUM(g_buf, frame_index)),
            frame_index);
    g_buf -> num_used_frame -= 1;
}

/* appending frame into TAIL of LRU list.
 * when it was requested from upper layer, this function will be called.
 */
void frame_append(int frame_index) {
    
    if (g_buf -> LRU_head == LRU_LIST_TAIL) {
        BUF_GET_FRAME(g_buf, frame_index).next = LRU_LIST_TAIL;
        BUF_GET_FRAME(g_buf, frame_index).prev = LRU_LIST_HEAD;
        g_buf -> LRU_head = frame_index;
        g_buf -> LRU_tail = frame_index;
    } else {
        BUF_GET_FRAME(g_buf, g_buf -> LRU_tail).next = frame_index;
        BUF_GET_FRAME(g_buf, frame_index).next = LRU_LIST_TAIL;
        BUF_GET_FRAME(g_buf, frame_index).prev = g_buf -> LRU_tail;
        g_buf -> LRU_tail = frame_index;
    }
    hash_insert(g_buf -> frame_in_usage,                                        
            hash_table_page(BUF_GET_TABLE_ID(g_buf, frame_index),               
                            BUF_GET_PAGE_NUM(g_buf, frame_index)),            
            frame_index);
    g_buf -> num_used_frame += 1;

}

/* pop frame for eviction.
 * if first frame is pinned, then frame_delete will be called instead.
 */
int frame_pop() {

    int ret_frame = g_buf -> LRU_head;

    if (ret_frame == LRU_LIST_TAIL)
        return -1;

    if (ret_frame == g_buf -> LRU_tail) {
        g_buf -> LRU_head = LRU_LIST_TAIL;
        g_buf -> LRU_tail = LRU_LIST_HEAD;
    } else {
        g_buf -> LRU_head = BUF_GET_FRAME(g_buf, ret_frame).next;
        BUF_GET_FRAME(g_buf, g_buf -> LRU_head).prev = LRU_LIST_HEAD;
    }

    return ret_frame;
}
