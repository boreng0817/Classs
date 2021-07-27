#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[]) 
{
    int count = 10;
    int pid = fork();
    while (count) {
        if (pid > 0) {
            printf(1, "Parent\n");
            //yield();
        } else if (pid == 0) {
            printf(1, "Child\n");
            //yield();
        } else {
            printf(1, "fork fail\n");
            break;
        }
        count--;
    }

    if (pid == 0)
        sleep(2);
    else if (pid > 0)
        wait();

    exit();
}
