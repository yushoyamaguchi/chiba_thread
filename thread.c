/*
  User Thread Library
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
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
    int			context[CONTEXT_SIZE];
    char*		stack_top;	/* NULL if this is main() thread */
    int			status;
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

static void LinkThread(Thread* t);
static void ThreadStart(int proc, int arg);
static Thread* AllocateThread();
static void FreeThread(Thread* t);

void main(int args, char** argv)
{
    ???
}

int ThreadCreate(ThreadProc proc, int arg)
{
    ???
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
static void ThreadStart(int proc, int arg)
{
    ThreadProc ptr = (ThreadProc)proc;
    (*ptr)(arg);
    ThreadExit();
}

/*
  ThreadYield() switches the context.
*/
void ThreadYield()
{
    ???
}

/*
  ThreadExit() finishes the current thread.
  If this thread is main(), this function waits until all tasks finish,
  then normally returns.
*/
void ThreadExit()
{
    ???
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
    free(t->stack_top);
    free(t);
}
