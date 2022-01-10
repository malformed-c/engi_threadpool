#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "engi_thread.h"
#include "engi_queue.h"
#include "engi_threadpool.h"

volatile sig_atomic_t engi_shutdown = 0;

void * simple_task(void *args)
{
	puts("simple_task");

	return;
}

void * work_test()
{
	static int c = 0;
	printf("test %d\n", c++);
	return NULL;
}

int main(int argc, char *argv[])
{
	engi_pool_t pool;

	engi_pool_init(&pool);

	for(unsigned i = 0; i < 2; i++)
	{
		engi_pool_worker_add(&pool);
	}



	for(unsigned i = 0; i < 1000; i++)
	{
		engi_task_t *work = calloc(1, sizeof(work));
		work->func = work_test;
		engi_pool_work_add(&pool, work);
	}

	sleep(3);

	engi_pool_destroy(&pool);

	sleep(1);

	exit(0);
}
