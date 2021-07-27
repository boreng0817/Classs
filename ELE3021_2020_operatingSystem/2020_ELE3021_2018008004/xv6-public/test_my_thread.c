#include "types.h"
#include "stat.h"
#include "user.h"

void*
basicthreadmain(void *arg)
{
    int tid = (int) arg;
    int i;
    for (i = 0; i < 1000000; ++i)
        if(i%10000 == 0)
            printf(1, "%d:%d\n", tid, i);
    thread_exit((void*)(tid+1));
}
int
main(int argc, char *argv[])
{
    thread_t thread;
    int i = 3;
    void *ret;

    if (thread_create(&thread, basicthreadmain, (void*)i) == 0)
      printf(1, "thread_create Success arg=[%d]\n", i);

    if (thread_join(thread, &ret) == 0)
      printf(1, "thread_join Success ret=[%d]\n", (int)ret);

    exit();
}
