#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct rw_lock
{
	int write_cnt;
	int read_cnt;
	//read mutex, cond
	pthread_mutex_t r_con_mutex;
	pthread_cond_t r_con;
	//write mutex, cond
	pthread_mutex_t w_con_mutex;
	pthread_cond_t w_con;
	//read, write가 모두 있어야 획득 가능한
	//write mutex <- 이게 있어야 이제 쓸 수 있다
	pthread_mutex_t w_mutex;
};

void init_rwlock(struct rw_lock * rw);
void r_lock(struct rw_lock * rw);
void r_unlock(struct rw_lock * rw);
void w_lock(struct rw_lock * rw);
void w_unlock(struct rw_lock * rw);
long *max_element(long* start, long* end);
long *min_element(long* start, long* end);
