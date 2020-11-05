#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct rw_lock
{
	int write_cnt;
	int write_wait;
	int read_cnt;
	pthread_mutex_t rw_mutex;
	pthread_cond_t r_cond;
	pthread_cond_t w_cond;
};

void init_rwlock(struct rw_lock * rw);
void r_lock(struct rw_lock * rw);
void r_unlock(struct rw_lock * rw);
void w_lock(struct rw_lock * rw);
void w_unlock(struct rw_lock * rw);
long *max_element(long* start, long* end);
long *min_element(long* start, long* end);
