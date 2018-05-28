#include <stdio.h>
#include <unistd.h>
#include "coroutine.h"

#define N 4096

semaphore_t sem;
void increase(void *step) {
    int stepn = *((int *) step);
    int tid = get_current_tid();
    for (int i = 0; i < 30; i += stepn) {
        usleep(1000*1000);
        if (tid == 1) {
            if (i == 20) {
                uthread_signal(&sem);
            }
        } else if (tid == 2) {
            if (i == 0) {
                uthread_suspend(1);
            } else if (i == 18) {
                uthread_resume(1);
            }
        } else if (tid == 3) {
            if (i == 0) {
                uthread_wait(&sem);
            }
        }
        printf("tid: %d - current value is %d and step is %d\n", tid, i, stepn);
        uthread_yield();
    }
}


int main() {
    int steps[] = {1, 3, 5};
    sem.value = 0;
    uthread_init();
    for (int i = 0; i < 3; i++) {
        uthread_spawn(increase, steps + i, N, 30 * SECOND * steps[i]);
    }
    uthread_start();
}

