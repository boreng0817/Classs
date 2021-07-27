#include "trx.h"
#include <pthread.h>
using namespace std;

char ret_val[200];

void* func1(void* arg) {
    int trx_id = begin_trx();
    
    char a[] = "qwer";

    db_update(1,333,a,trx_id);
    db_update(1,334,a,trx_id);
    db_update(1,335,a,trx_id);
    db_update(1,336,a,trx_id);
    db_update(1,337,a,trx_id);
    db_find(1,1,ret_val,trx_id); printf("%s     ", ret_val);cout << 2 << endl;
    db_update(1,17,a,trx_id); cout << 5 << endl;
    end_trx(trx_id);
    return NULL;
}

void* func2(void* arg) {
    int trx_id = begin_trx();
    char a[] = "zxcv";
    char b[] = "a11sdiofn";

    db_find(1,1,ret_val, trx_id); printf("%s    ", ret_val); cout << 1 << endl;
    db_update(1,17,a, trx_id); cout << 3 << endl;
    db_update(1,1,b, trx_id); cout << 4 << endl;
    end_trx(trx_id);
    return NULL;
}

void* func(void* arg) {
    int tid = begin_trx();

    char a[] = "hahah";
    db_update(1,499,a, tid);
    db_update(1,500,a,tid);
    end_trx(tid);

    return NULL;
}

void* a(void* arg) {
    int tid = begin_trx();
    char a[] = "ABCD";
    char b[] = "CDE";
    sleep(1);
    db_update(1,1,a, tid); cout << 2 << endl;
    db_update(1,3,b, tid); cout << 3 << endl;
    sleep(3);
    end_trx(tid); cout << "a ends" << endl;
}

void* b(void* arg) {
    int tid = begin_trx();
    char a[] = "AD";
    db_update(1,3,a, tid); cout << 1 << endl;
    for (int i = 0; i < 20; ++i)
        db_update(1,30, a, tid);
    sleep(2);
    db_update(1,1,a, tid); cout << 4 << endl;
    end_trx(tid); cout << "b ends" << endl;
}
void gdb() {
    return;
}

int main() {

    char path[] = "DATA1";
    init_db(100);
    open_table(path);
 
    pthread_t threads[10];

//    pthread_create(&threads[1], 0, func2, NULL);
    pthread_create(&threads[0], 0, a, NULL);
    pthread_create(&threads[1], 0, b, NULL);

    int cnt = 2;
/*
    for (int i = 0; i < cnt; ++i) {
        pthread_create(&threads[i], 0, func, NULL);
        gdb();
    }
*/
    for (int i = 0; i < cnt; ++i)
        pthread_join(threads[i], NULL);

    shutdown_db();
    return 0;
}
