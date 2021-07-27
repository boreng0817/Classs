#include "buf.h"

buffer g_buf;
int g_fd;

int initiate_table(char *filename) {
    int i, fd, unique_id;
    free_page_t free;
    header_page_t head;
    
    memset(&head, 0, PAGE_SIZE);
    memset(&free, 0, PAGE_SIZE);

    head.free_page_num = INITIAL_PAGE_NUM - 1;
    head.root_page_num = 0;
    head.num_of_pages = INITIAL_PAGE_NUM;

    fd = open(filename, O_RDWR | O_CREAT, 0644);

    for (i = 1; i <= MAX_TABLE_SIZE; ++i) {
        if (g_buf -> table_ids[i] == BUF_EMPTY_TABLE) {
            g_buf -> table_ids[i] = simple_hash(filename);
            g_buf -> fds[i] = fd;
            g_buf -> table_num += 1;
            unique_id = i;
            break;
        }
    }

    buf_write_page(g_buf -> table_num, 0, CAST_TO_PAGE(head));

    for (i = 0 ; i < INITIAL_PAGE_NUM - 1 ; ++i ) {
        free.next_free_page_num = i;
        buf_write_page(g_buf -> table_num, i + 1, (page_t*)&free);
    }

    return unique_id; // unique value
}

int _open_table(char *pathname) {
    
    int hash_digest = simple_hash(pathname);
    int i, fd;

    for (i = 1; i <= MAX_TABLE_SIZE ; ++i) {
        if (hash_digest == g_buf -> table_ids[i])
            break;
    }
    if (i != MAX_TABLE_SIZE + 1) {
        printf("Already opened\n");
        return -1;
    } else if (g_buf -> table_num == MAX_TABLE_SIZE) {
        printf("Opened 10 tables already\n");
        return -1;
    }

    fd = open(pathname, O_RDWR);
    
    if (fd < 0) {
        return initiate_table(pathname);
    } else {
        for (i = 1; i <= MAX_TABLE_SIZE; ++i) {
            if (g_buf -> table_ids[i] == BUF_EMPTY_TABLE) {
                g_buf -> table_ids[i] = simple_hash(pathname);
                g_buf -> fds[i] = fd;
                g_buf -> table_num += 1;
                return i; // unique value
            }
        }
    }
}

int _close_table(int table_id) {
 
    int i, fd;
    
    if (table_id < 1 || table_id > MAX_TABLE_SIZE)
        return -1;
    
    if (g_buf -> table_ids[table_id] == BUF_EMPTY_TABLE)
        return -1;

    for (i = 0; i < g_buf -> capacity; ++i) {
        if (BUF_GET_FRAME(g_buf, i).table_id == table_id)
            buf_evict_frame(i);
    }

    fd = g_buf -> fds[table_id];
    close(fd);
    g_buf -> table_ids[table_id] = BUF_EMPTY_TABLE;
    g_buf -> table_num -= 1;
    return 0;
}

int simple_hash(char *file_name) {
    
    int i, ret = 0;

    for (i = 0 ; i < strlen(file_name) ; ++i) {
        ret = ((file_name[i] - 'A') + ret) * (i + 1) % 100000000;
    }

    return ret;
}

