#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	//mutex, cond 선언 및 초기화
	pthread_mutex_init(&rw->rw_mutex, NULL);
	pthread_cond_init(&rw->r_cond, NULL);
	//read, write 카운트 변수 초기화
	rw->read_cnt = 0;
	rw->write_cnt = 0;

	return;
}

void r_lock(struct rw_lock * rw)
{

	//read lock은 누구나 획득 가능
	pthread_mutex_lock(&rw->rw_mutex);
	rw->read_cnt++;
	pthread_mutex_unlock(&rw->rw_mutex);
}

void r_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	rw->read_cnt--;
	//아무도 read하고 있지 않을때
	//write_thread 활성화
	if(rw->read_cnt == 0){
		pthread_cond_signal(&rw->r_cond);
	}

	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	//read나 write하고 있는 쓰레드가 존재하면 대기 시킨다
	if(rw->read_cnt > 0 || rw->write_cnt > 0){
		pthread_cond_wait(&rw->r_cond, &rw->rw_mutex);
	}
	rw->write_cnt++;

	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	rw -> write_cnt--;
	//쓰기를 완료하면 대기 중인 다른 쓰레드를 깨움
	if(rw->write_cnt == 0){
		pthread_cond_signal(&rw->r_cond);
	}
	pthread_mutex_unlock(&rw->rw_mutex);

}
