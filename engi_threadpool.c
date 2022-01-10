#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "engi_threadpool.h"

volatile engi_pool_t *pool_p;

extern int engi_shutdown;

void * wake_main(void *args)
{
	engi_pool_t *pool = (engi_pool_t *)args;
	engi_pool_queue_t *tq = &pool->task_q;

	pthread_cond_t *cv = &tq->cond;
	pthread_mutex_t *cvm = &tq->cond_mutex;

	while(unlikely(engi_shutdown == 0))
	{
		unsigned int t = tq->num;
		while(t-- >= 0)
		{
			pthread_mutex_lock(&tq->cond_mutex);
			pthread_cond_signal(&tq->cond);
			pthread_mutex_unlock(&tq->cond_mutex);
		}

		sleep(1);
	}


}

void * worker_main(void *args)
{
	engi_task_t *ret = NULL;

	engi_pool_t *pool = (engi_pool_t *)args;
	engi_pool_queue_t *tq = &pool->task_q;

	pthread_cond_t *cv = &tq->cond;
	pthread_mutex_t *cvm = &tq->cond_mutex;

	pthread_mutex_t *tqm = &tq->mutex;
	engi_queue_t *tqq = &tq->queue;

	while(unlikely(engi_shutdown == 0))
	{
		engi_task_t *work = {0};

		if(*tq->num == 0) //block if there is no task avaible
		{
			pthread_mutex_lock(cvm);
			pthread_cond_wait(cv, cvm);
		}

		pthread_mutex_lock(tqm);
		if(likely(*tq->num > 0))
		{
			work = (engi_task_t *)tqq->dequeue(tqq, NULL);
		}
		pthread_mutex_unlock(tqm);

		pthread_mutex_unlock(cvm);
		pthread_cond_signal(cv); //signal other thread that mutex is unlocked


		if(likely(work != NULL))
		{
			ret = work->func(work->args);

			pthread_mutex_lock(tqm);
			if(tqq->enqueue != NULL) tqq->enqueue(tqq, ret);
			pthread_mutex_unlock(tqm);

			free(work);
		}
	}

	return ret;
}

int engi_pool_queue_init(engi_pool_queue_t *self)
{
	int res = 0;

	res += pthread_mutex_init(&self->mutex, NULL);
	res += pthread_cond_init(&self->cond, NULL);
	res += pthread_mutex_init(&self->cond_mutex, NULL);
	res += pthread_barrier_init(&self->barrier, NULL, 4);
	res += engi_queue_init(&self->queue);

	self->num = &self->queue.size;

	return res;
}

void engi_pool_queue_destroy(engi_pool_queue_t *self)
{
	engi_queue_destroy(&self->queue);

	pthread_mutex_lock(&self->cond_mutex);
	pthread_cond_broadcast(&self->cond);
	pthread_mutex_unlock(&self->cond_mutex);

	pthread_mutex_lock(&self->cond_mutex);

	pthread_mutex_destroy(&self->mutex);
	pthread_mutex_destroy(&self->cond_mutex);
	pthread_cond_destroy(&self->cond);
	pthread_barrier_destroy(&self->barrier);

}

int engi_pool_init(engi_pool_t *self)
{
	int res = 0;

	res += engi_pool_queue_init(&self->task_q);

	pool_p = self;

	self->shutdown = 0;

// 	engi_thread_t wake;
// 	engi_task_t work = {.func = wake_main, .args = self};
//
// 	engi_thread_init(&wake);
// 	engi_thread_create(&wake, &work);
// 	pthread_detach(wake.thread);

	return res;
}

void engi_pool_destroy(engi_pool_t *self)
{
	self->shutdown = 1;

	engi_pool_queue_destroy(&self->task_q);

}

int engi_pool_worker_add(engi_pool_t *self)
{
	engi_thread_t worker;
	engi_task_t work = {.func = worker_main, .args = self};

	engi_thread_init(&worker);
	engi_thread_create(&worker, &work);
	pthread_detach(worker.thread);

	return 0;
}

int engi_pool_work_add(engi_pool_t *self, engi_task_t *work)
{
	engi_pool_queue_t *tq = &self->task_q;
	pthread_mutex_t *tqm = &tq->mutex;
	engi_queue_t *tqq = &tq->queue;

	pthread_mutex_lock(tqm);
	tqq->enqueue(tqq, work);
	pthread_mutex_unlock(tqm);

	pthread_mutex_lock(&tq->cond_mutex);
	pthread_cond_signal(&tq->cond);
	pthread_mutex_unlock(&tq->cond_mutex);

	return 0;
}
