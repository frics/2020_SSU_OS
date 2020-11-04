#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	pthread_mutex_init(&rw->rw_mutex, NULL);
	pthread_cond_init(&rw->r_cond, NULL);
	pthread_cond_init(&rw->w_cond, NULL);

	rw->read_cnt = 0;
	rw->write_cnt = 0;
	rw->write_wait = 0;
	return;
}

void r_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);

	if(rw->write_cnt > 0 || rw->write_wait >0){
		pthread_cond_wait(&rw->r_cond, &rw->rw_mutex);
	}
	rw->read_cnt++;
	pthread_mutex_unlock(&rw->rw_mutex);
}

void r_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	rw->read_cnt--;

	if(rw->write_wait > 0 && rw->read_cnt == 0){
		pthread_cond_signal(&rw->w_cond);
	}else if(rw->write_wait == 0 && rw->read_cnt ==0){
		pthread_cond_signal(&rw->r_cond);
	}
	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	if(rw->read_cnt > 0 || rw->write_cnt >0){
		rw->write_wait++;
		pthread_cond_wait(&rw->w_cond, &rw->rw_mutex);
		rw->write_wait--;
	}
	rw->write_cnt++;
	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	rw->write_cnt--;

	if(rw->write_wait > 0){
		pthread_cond_signal(&rw->w_cond);
	}else if(rw->write_wait ==0){
		pthread_cond_broadcast(&rw->r_cond);
	}
	pthread_mutex_unlock(&rw->rw_mutex);

}
