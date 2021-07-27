#include "bpt.h"
#include "buf.h"

//buffer g_buf;

/* Reading a page from file. Pagenum becomes index.
 */
void file_read_page(int tablenum, pagenum_t pagenum, page_t* dest) {

    int fd = g_buf -> fds[tablenum];
    pread(fd, dest, PAGE_SIZE, pagenum*PAGE_SIZE);
    return;
}

/* Writing a page into file.
 */
void file_write_page(int tablenum, pagenum_t pagenum, const page_t* src) {
    int fd = g_buf -> fds[tablenum];
    pwrite(fd, src, PAGE_SIZE, pagenum*PAGE_SIZE);
    return;
}
