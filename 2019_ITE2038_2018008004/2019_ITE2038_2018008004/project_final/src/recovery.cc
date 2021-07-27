#include "recovery.h"
using namespace std;

int recovery(char *logfile) {

    int fd = open(logfile, O_RDWR);
    int read_byte, i, page_size = g_log_buffer -> buffer_size;

    if (fd < 0) {
        printf("nothing to recover\n");
        return 1;
    }

    //analysis & redo part
    while ( 1 ) {
        read_byte = pread(fd, g_log_buffer -> log_buffer, 
                page_size, page_size * i);
        i += 1;
        redo_phase(g_log_buffer, read_byte/sizeof(log_record_t));
        if (read_byte != page_size)
            break;
    }

    //undo part
    undo_phase(g_log_buffer);

    return 1;
}

void redo_phase(log_buffer_t *buf, int record_count) {
    
    if (!record_count)
        return;

    int i;
    log_record_t log;

    for (i = 0; i < record_count; ++i) {
        log = *(buf -> log_buffer[i]);
        
        switch (log.type) {

            case BEGIN:
                buf -> analysis_table[log.trx_id] = LOSER;
                break;

            case UPDATE:
                redo(log);
                break;

            case COMMIT:
                buf -> analysis_table[log.trx_id] = WINNER;

            case ABORT:
                // to do
                break;

            default:
                printf("error occured. can't reach here\n");
        }
    }
}

void redo(log_record_t log) {

}

log_buffer_t *init_log_buffer(int size) {

    log_buffer_t *buf = (log_buffer_t*)malloc(sizeof(log_buffer_t));

    buf -> size = 0;
    buf -> capacity = size;
    buf -> buffer_size = sizeof(log_record_t) * size;
    buf -> next_LSN = 0;
    buf -> log_buffer = (log_record_t*)malloc(sizeof(log_record_t) * size);
    buf -> log_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

    return buf;
}

int log_buffer_flush(log_buffer_t *buf) {

}
