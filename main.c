#include <stdio.h>
#include "coroutine.h"

#define N 4096
void increase(void *step) {
    int stepn = *((int *) step);
    for (int i = 0; i < 30; i += stepn) {
        printf("current value is %d and step is %d\n", i, stepn);
        uthread_yield();
    }
}


int main() {
    int steps[] = {2, 3, 5};
    uthread_init();
    for (int i = 0; i < 3; i++) {
        uthread_spawn(increase, steps + i, N);
    }
    uthread_start();
}

