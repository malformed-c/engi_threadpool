#pragma once

#include <pthread.h>

#include "engi_queue.h"
#include "engi_thread.h"

#include "common.h"

typedef struct engi_pool_queue_s
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_mutex_t cond_mutex;
	pthread_barrier_t barrier;
	engi_queue_t queue;
	unsigned int *num;
} engi_pool_queue_t;

typedef struct engi_pool_s
{
	engi_pool_queue_t task_q;
	char shutdown:1;
} engi_pool_t;

int engi_pool_queue_init(engi_pool_queue_t *self);
void engi_pool_queue_destroy(engi_pool_queue_t *self);

int engi_pool_init(engi_pool_t *self);
void engi_pool_destroy(engi_pool_t *self);

int engi_pool_worker_add(engi_pool_t *self);

int engi_pool_work_add(engi_pool_t *self, engi_task_t *work);
void * engi_pool_work_grab(engi_pool_t *self);
