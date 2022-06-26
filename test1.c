#include <stdio.h>
#include "thread.h"

void f(int i)
{
    for(int k=0;k<100000000;k++){
      if(k%40000000==0){
        printf("thread %d running\n ",i);
      }
    }
    printf("thread (i=%d) finished.\n", i);
}

void ThreadMain()
{
    int t1, t2;

    t1 = ThreadCreate(f, 1);
    printf("create a new thread (i=%d) [id=%d]\n", 1, t1);
    t2 = ThreadCreate(f, 2);
    printf("create a new thread (i=%d) [id=%d]\n", 2, t2);
    ThreadYield();
    printf("main thread finished.\n");
}
