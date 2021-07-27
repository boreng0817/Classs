#include "trx.h"

typedef uint64_t LogSequenceNumber;
enum logType {BEGIN, UPDATE, COMMIT, ABORT};
enum analysisState {WINNER, LOSER};
typedef struct log_record_t {
    LogSequenceNumber LSN;
    LogSequenceNumber prev_LSN;
    int trx_id;
    logType type;
    int table_id;
    int page_num;
    int offset;
    int data_length;

    char old_image[120];
    char new_image[120];
} log_record_t;

typedef struct log_buffer_t {

    int size;
    int capacity;
    int buffer_size;
    LogSequenceNumber next_LSN;
    log_record_t *log_buffer;
    pthread_mutex_t log_buffer_mutex;
    unordered_map<int, analysisState> analysis_table;
    int fd_log_file;

} log_buffer_t;

/* function */
int recovery(char *logfile);
void redo_phase(log_buffer_t *buf, int record_count);
void redo(log_record_t log);
void undo_phase(log_buffer_t *buf);
log_buffer_t *init_log_buffer(int size);
int log_buffer_flush(log_buffer_t *buf);




