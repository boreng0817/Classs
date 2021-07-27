#include "bpt.h"

int g_fd;
header_page_t g_head;

int initiate_table(char *filename) {
    int i;
    free_page_t free;
    
    memset(&g_head, 0, PAGE_SIZE);

    g_head.free_page_num = INITIAL_PAGE_NUM - 1;
    g_head.root_page_num = 0;
    g_head.num_of_pages = INITIAL_PAGE_NUM;

    g_fd = open(filename, O_RDWR | O_CREAT, 0644);

    file_write_page(0, (page_t*)&g_head);

    memset(&free, 0, PAGE_SIZE);
    for (i = 0 ; i < INITIAL_PAGE_NUM - 1 ; ++i ) {
        free.next_free_page_num = i;
        file_write_page(i + 1, (page_t*)&free);
    }

    return simple_hash(filename); // unique value
}

int open_table(char *pathname) {
    
    g_fd = open(pathname, O_RDWR);

    if (g_fd < 0) {
        return initiate_table(pathname);
    } else {
        file_read_page(0, (page_t*)&g_head);
        return simple_hash(pathname); // unique value
    }
    
    //open fail
    return -1;
}

void close_table() {
    close(g_fd);
}

int simple_hash(char *file_name) {
    
    int i, ret = 0;

    for (i = 0 ; i < strlen(file_name) ; ++i) {
        ret = ((file_name[i] - 'A') + ret) * (i + 1) % 100000000;
    }

    return ret;
}

