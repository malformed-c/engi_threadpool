#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "engi_thread.h"
#include "engi_queue.h"
#include "engi_pool.h"

#define T_NUM 16

void * simple_task(void *args)
{
	char *str = args;

	printf("%s\n", str);

	return("simple task done");
}

int main(int argc, char *argv[])
{
	engi_pool_t pool;

	engi_pool_init(&pool);

	for(unsigned i = 0; i < 16; i++)
	{
		engi_pool_worker_add(&pool);
	}

	engi_task_t work = {simple_task, "main"};

	for(unsigned i = 0; i < 1024; i++)
	{
		engi_pool_work_add(&pool, &work);

	}

	sleep(10);

	void *work_res = NULL;

	unsigned int count = 0;

	for(unsigned i = 0; i < *pool.comp_q.num; i++)
	{
		work_res = engi_pool_grabber_grab(&pool);
		if(work_res != NULL)
		{
			printf("%s\n", (char *)work_res);
		}
		count = i;
	}

	printf("%d\n", count);

	engi_pool_destroy(&pool);

	exit(0);
}
