#include "bpt.h"

int g_fd;
header_page_t g_head;

/* Function for doubling file size 
 */
void expand_file() {

    pagenum_t size_expand_to, file_num;
    free_page_t free;
    int i;

    size_expand_to = g_head.num_of_pages * 2;
    
    memset(&free, 0, PAGE_SIZE);
    free.next_free_page_num = 0;
    file_write_page(g_head.num_of_pages, (page_t*)&free);

    for (i = g_head.num_of_pages + 1; i < size_expand_to ; ++i) {
        free.next_free_page_num = i - 1;
        file_write_page(i, (page_t*)&free);
    }

    g_head.free_page_num = size_expand_to - 1;
    g_head.num_of_pages = size_expand_to;

    file_write_page(0, (page_t*)&g_head);
    
    return;
}

/* Allocate free_page for purpose. 
 */
pagenum_t file_alloc_page () {

    pagenum_t ret_pagenum;
    free_page_t free;

    if( g_head.free_page_num == 0 )
        expand_file();

    ret_pagenum = g_head.free_page_num;
    file_read_page(ret_pagenum, (page_t*)&free);
    g_head.free_page_num = free.next_free_page_num;
    file_write_page(0, (page_t*)&g_head);

    return ret_pagenum;
}

/* Free used page into free page.
 * clean up garbage data.
 */
void file_free_page (pagenum_t pagenum) {

    free_page_t free;

    memset(&free, 0, PAGE_SIZE);
    free.next_free_page_num = g_head.free_page_num;
    g_head.free_page_num = pagenum;

    file_write_page(0, (page_t*)&g_head);
    file_write_page(pagenum, (page_t*)&free);

    return;
}

/* Reading a page from file. Pagenum becomes index.
 */
void file_read_page (pagenum_t pagenum, page_t* dest) {

    pread(g_fd, dest, PAGE_SIZE, pagenum*PAGE_SIZE);
    return;
}

/* Writing a page into file.
 */
void file_write_page (pagenum_t pagenum, const page_t* src) {
    
    pwrite(g_fd, src, PAGE_SIZE, pagenum*PAGE_SIZE);
    return;
}
