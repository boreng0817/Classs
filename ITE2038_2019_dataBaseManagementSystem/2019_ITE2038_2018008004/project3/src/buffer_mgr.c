#include "buf.h"

/* global variable */
buffer g_buf;

/* namely, initiate buffer with size of [size].
 * detailed structure for buffer will be explained
 * in WIKI.
 */
buffer buf_init(int size) {

    buffer buf = (buffer)malloc(sizeof(Buffer));
    int i;

    buf -> capacity = size;
    buf -> num_used_frame = 0;
    buf -> frames = (frame*)malloc(sizeof(frame) * size);

    for (i = 0; i < size; ++i) {
        memset(&(buf -> frames[i]), 0, SIZE_OF_FRAME);
    }

    buf -> fds = (int*)malloc(sizeof(int) * (MAX_TABLE_SIZE + 1));
    buf -> table_ids = (int*)malloc(sizeof(int) * (MAX_TABLE_SIZE + 1));
    for (i = 1; i <= MAX_TABLE_SIZE; ++i) {
        buf -> table_ids[i] = BUF_EMPTY_TABLE;
    }
    buf -> table_num = 0;
    buf -> empty_frame = stack_init(size);
    for (i = 0; i < size; ++i) {
        buf -> empty_frame -> arr[i] = i;
    }
    buf -> empty_frame -> num_element = size;

    buf -> LRU_head = LRU_LIST_TAIL;
    buf -> LRU_tail = LRU_LIST_HEAD;

    buf -> frame_in_usage = hash_init(size);
    buf -> table_buffer = (stack*)malloc(sizeof(Stack) * (MAX_TABLE_SIZE + 1));

    /* not used -> should be modified for further project */
    for (i = 1; i <= MAX_TABLE_SIZE; ++i) {
        buf -> table_buffer[i] = stack_init(size);
    }

    return buf;
}

/* expand table if there is no more free page
 * in the [table_num] table.
 */
void buf_expand_file(tablenum_t table_num) {

    pagenum_t size_expand_to, file_num;
    free_page_t free;
    header_page_t head;
    int i;

    buf_set_pin(
            buf_read_page(table_num, 0, CAST_TO_PAGE(head)));
    size_expand_to = head.num_of_pages * 2;

    memset(&free, 0, PAGE_SIZE);
    free.next_free_page_num = 0;
    buf_write_page(table_num, head.num_of_pages, CAST_TO_PAGE(free));

    for (i = head.num_of_pages + 1; i < size_expand_to; ++i) {
        free.next_free_page_num = i - 1;
        buf_write_page(table_num, i, CAST_TO_PAGE(free));
    }

    head.free_page_num = size_expand_to - 1;
    head.num_of_pages = size_expand_to;

    buf_unset_pin(
            buf_write_page(table_num, 0, CAST_TO_PAGE(head)));

    return;
}

/* alloc a free page from [table_num] table.
 * update header page.
 */
pagenum_t buf_alloc_page(tablenum_t table_num) {

    header_page_t head;
    free_page_t free;
    pagenum_t ret_pagenum;
    int head_frame = buf_read_page(table_num, 0, CAST_TO_PAGE(head));

    if (head.free_page_num == 0) {
        buf_expand_file(table_num);
        buf_read_page(table_num, 0, CAST_TO_PAGE(head));
    }

    ret_pagenum = head.free_page_num;
    buf_read_page(table_num, ret_pagenum, CAST_TO_PAGE(free));
    head.free_page_num = free.next_free_page_num;
    buf_write_page(table_num, 0, CAST_TO_PAGE(head));
    return ret_pagenum;
}

/* free allocated [table_num] page into free page 
 * int [table_num] table.
 */
void buf_free_page(tablenum_t table_num, pagenum_t page_num) {

    header_page_t head;
    free_page_t free;

    memset(&free, 0, PAGE_SIZE);
    buf_read_page(table_num, 0, CAST_TO_PAGE(head));

    free.next_free_page_num = head.free_page_num;
    head.free_page_num = page_num;

    buf_write_page(table_num, 0, CAST_TO_PAGE(head));
    buf_write_page(table_num, page_num, CAST_TO_PAGE(free));

    return;
}

/* first, check if frame is in the buffer.
 * if is, give to upper layer.
 * if not, read request read to file_mgr [table_num].[page_num]
 */
int buf_read_page(tablenum_t table_num, pagenum_t page_num, page_t *dest) {

    int frame_index;

    /*first, check page exists in buffer*/
    frame_index = hash_get_frame(g_buf -> frame_in_usage, 
            table_num, 
            page_num); 

    /* when found, just append into LRU list's tail. */
    if (frame_index != FRAME_NOT_FOUND) {
        frame_delete(frame_index);
        frame_append(frame_index);
    } 

    /* if not, do eviction */
    else {
        if (STACK_IS_EMPTY(g_buf -> empty_frame)) 
            buf_evict_frame(-1);

        frame_index = stack_pop(g_buf -> empty_frame);

        file_read_page(table_num, page_num,
                CAST_TO_PAGE(BUF_GET_FRAME(g_buf, frame_index).page));
        BUF_GET_FRAME(g_buf, frame_index).page_num = page_num;
        BUF_GET_FRAME(g_buf, frame_index).table_id = table_num;
        frame_append(frame_index);
    }

    memcpy(dest, &BUF_GET_FRAME(g_buf, frame_index).page, PAGE_SIZE);
    return frame_index;
}

/* first, check if frame is in the buffer. "probably in buffer"
 * if is, write into buffer.
 * if not, read page through requesting to file_mgr, then write into buffer.
 */
int buf_write_page(tablenum_t table_num, pagenum_t page_num, page_t *src) {

    int frame_index;

    frame_index = hash_get_frame(g_buf -> frame_in_usage,                       
                                  table_num, page_num);

    if (frame_index != FRAME_NOT_FOUND) {
        frame_delete(frame_index);                                              
        frame_append(frame_index);
    } else {
        if (STACK_IS_EMPTY(g_buf -> empty_frame))                               
            buf_evict_frame(-1);

        frame_index = stack_pop(g_buf -> empty_frame);
        file_read_page(table_num, page_num, 
                &BUF_GET_FRAME(g_buf, frame_index).page);
        BUF_GET_FRAME(g_buf, frame_index).page_num = page_num;                  
        BUF_GET_FRAME(g_buf, frame_index).table_id = table_num;
        frame_append(frame_index);
    }

    memcpy(&BUF_GET_FRAME(g_buf, frame_index).page, src, PAGE_SIZE);       
    BUF_GET_FRAME(g_buf, frame_index).is_dirty = 1;
    return frame_index;
}

/* when there is no empty frame for R/W requests, 
 * evict the first (header) element in LRU list.
 * it can be used to indicate index of buffer frame if [index] != -1.
 */
void buf_evict_frame(int index) {

    int frame_index_evict;

    if (index == -1) {
        frame_index_evict = g_buf -> LRU_head;

        while (frame_index_evict != LRU_LIST_TAIL &&
               BUF_GET_FRAME(g_buf, frame_index_evict).is_pinned) {
            frame_index_evict = BUF_GET_FRAME(g_buf, frame_index_evict).next;
        }
        /* all frames are pinned. need larger buffer! */
        if (frame_index_evict == LRU_LIST_TAIL) {
            /* expand buffer */
        }
    }
    else
        frame_index_evict = index;

    if (BUF_GET_FRAME(g_buf, frame_index_evict).is_dirty) {
        file_write_page(BUF_GET_FRAME(g_buf, frame_index_evict).table_id,
                BUF_GET_FRAME(g_buf, frame_index_evict).page_num,
                CAST_TO_PAGE(BUF_GET_FRAME(g_buf, frame_index_evict).page));
        BUF_GET_FRAME(g_buf, frame_index_evict).is_dirty = 0;
    }

    frame_delete(frame_index_evict);
    memset(&BUF_GET_FRAME(g_buf, frame_index_evict), 0, SIZE_OF_FRAME);
    stack_push(g_buf -> empty_frame, frame_index_evict);
}

/* pinning a frame for not to evict.
 */
void buf_set_pin(int frame_num) {

    g_buf -> frames[frame_num].is_pinned = 1;
}

/* unpinning a frame which was pinned.
 */
void buf_unset_pin(int frame_num) {

    g_buf -> frames[frame_num].is_pinned = 0;
}

/* free g_buf when program executes shutdown_db.
 */
void buf_free() {

    int i;

    free(g_buf -> fds);
    free(g_buf -> table_ids);
    stack_free(g_buf -> empty_frame);
    hash_free(g_buf -> frame_in_usage);

    for (i = 1; i <= MAX_TABLE_SIZE; ++i) 
        stack_free(g_buf -> table_buffer[i]);
    free(g_buf -> table_buffer);

}
