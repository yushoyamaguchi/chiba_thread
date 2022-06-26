/*
  User Thread Library
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <sys/time.h>
#include <setjmp.h>
#include "thread.h"

#define DEBUG(s)	puts(s)

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define	CONTEXT_SIZE	5
#define STACK_SIZE	(4096 * 8)

typedef struct _Thread {
    struct _Thread*	next;
    int			thread_id;
    //int			context[CONTEXT_SIZE];
    char*		stack_top;	/* NULL if this is main() thread */
    int			status;
    jmp_buf jmp_context;
} Thread;

#define RUNNING		0
#define FINISH		1

static Thread* currentThread;
static Thread* threadList;

/* in csw-i386.S */
void _ContextSwitch(int* old_context, int* new_context);
void _MakeThread(int* context, char* stack, void (*func)(int, int),
		 int arg1, int arg2);
int _TestAndSet(int* lock);

void ThreadMain();
static void LinkThread(Thread* t);
static void ThreadStart(void* proc, int arg,Thread* child);
static Thread* AllocateThread();
static void FreeThread(Thread* t);

void main(int args, char** argv)
{
    Thread* main_thread=AllocateThread();
    threadList=main_thread;
    currentThread=main_thread;
    ThreadMain();
    ThreadExit();
    FreeThread(main_thread);
}

int ThreadCreate(ThreadProc proc, int arg)
{
    intptr_t empty[1024];
    Thread *child=AllocateThread();
    Thread* search;
    for(search=threadList;search->next!=NULL;search=search->next){
    }
    search->next=child;
    ThreadStart(proc,arg,child);
    return child->thread_id;
}

static void LinkThread(Thread* t)
{
    Thread* tmp;

    tmp = threadList;
    threadList = t;
    threadList->next = tmp;
}

/*
  ThreadStart() is a procedure that threads first invoke.
*/
static void ThreadStart(void* proc, int arg, Thread *child)
{
    //intptr_t empty[1024];
    ThreadProc ptr = (ThreadProc)proc;
    printf("thread_init : id=%d\n",child->thread_id);
    int result=setjmp(child->jmp_context);
    if(result==0){
      return;
    }
    else{
      printf("func start : id=%d\n",child->thread_id);
    }
    (*ptr)(arg);
    ThreadExit();
    return;
}

/*
  ThreadYield() switches the context.
*/
void ThreadYield()
{
    Thread* search;
    int find=0;
    int current_id=currentThread->thread_id;
    for(search=threadList;search!=NULL;search=search->next){
        if(search->status==RUNNING&&current_id!=search->thread_id){
            find=1;
            break;
        }
    }
    if(find==1){
        Thread* cur = currentThread;
        currentThread = search;
        printf("old tid=%d , new tid=%d\n",cur->thread_id,currentThread->thread_id);
        int result=setjmp(cur->jmp_context);
        if(result==0){
          longjmp(search->jmp_context,2);
        }
        //_ContextSwitch(cur->context, t->context);
    }
    else if(currentThread->thread_id==1&&currentThread->status==FINISH){

    }
    return;
}

/*
  ThreadExit() finishes the current thread.
  If this thread is main(), this function waits until all tasks finish,
  then normally returns.
*/
void ThreadExit()
{
    static Thread dummy;
    Thread* cur = currentThread;
    printf("ThreadExit : id=%d\n",cur->thread_id);
    if (cur->thread_id==1){
        cur->status = FINISH;
    }
    else {
        //threadList が指すリスト構造から取り除く。
        Thread *search;
        int i=0;
        for(search=threadList;search->next!=NULL;search=search->next){
            if(search==cur&&i==0){
              threadList=cur->next;
              break;
            }
            if(search->next==cur){
                search->next=cur->next;
                break;
            }
            i++;
        }
        dummy.status = FINISH;
        dummy.stack_top = NULL;    /* 使用不可 */
        currentThread = &dummy;
        //free(cur->stack_top);
        free(cur);
    }
    ThreadYield();
}

static Thread* AllocateThread()
{
    static int n = 1;
    Thread* t;

    if ((t = (Thread*)malloc(sizeof(Thread))) == NULL) {
	     DEBUG("*** error: fail to allocate a thread");
	      exit(1);	/* error: no memory */
    }

    t->next = NULL;
    t->thread_id = n++;
    t->stack_top = NULL;
    t->status = RUNNING;
    return t;
}

static void FreeThread(Thread* t)
{
    //free(t->stack_top);
    free(t);
}


void f2(int i)
{
    for(int k=0;k<100000000;k++){
      if(k%40000000==0){
        printf("f2:thread %d running:k=%d\n ",i,k);
      }
      if(k%70000000==0){
        ThreadYield();
      }
    }
    printf("f2:thread (i=%d) finished.\n", i);
}

void f(int i)
{
    for(int k=0;k<100000000;k++){
      if(k%40000000==0){
        printf("thread %d running:k=%d\n ",i,k);
      }
      if(k%70000000==0){
        //ThreadYield();
      }
    }
    printf("thread (i=%d) finished.\n", i);
}

void ThreadMain()
{
    int t1, t2;

    t1 = ThreadCreate(f2, 2);
    printf("create a new thread (i=%d) [id=%d]\n", 2, t1);
    ThreadYield();
    //t2 = ThreadCreate(f, 3);
    //printf("create a new thread (i=%d) [id=%d]\n", 3, t2);
    //ThreadYield();
    printf("main thread finished.\n");
}
