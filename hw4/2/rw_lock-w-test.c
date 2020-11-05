#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	//mutex, cond 선언 및 초기화
	pthread_mutex_init(&rw->rw_mutex, NULL);
	pthread_cond_init(&rw->r_cond, NULL);
	pthread_cond_init(&rw->w_cond, NULL);
	//read, write 카운트 변수 및 write 대기 변수 초기화
	rw->read_cnt = 0;
	rw->write_cnt = 0;
	rw->write_wait = 0;
	return;
}

void r_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	//write thread가 활성화 되어 있거나
	//write를 대기중인 쓰레드가 있을 경우
	//read lock을 획득할 수 없음 -> 대기 
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
	
	//더 이상 read lock을 획득하고 있는 thread가 없고
	//write lock을 획득하기 위해 대기중인 쓰레드가 있을 경우
	//write thread를 깨워준다.
	if(rw->write_wait > 0 && rw->read_cnt == 0){
		pthread_cond_signal(&rw->w_cond);
	}
	//대기중인 write thread가 없는 경우 read thread들을 깨워준다.
	else if(rw->write_wait == 0 && rw->read_cnt ==0){
		pthread_cond_broadcast(&rw->r_cond);
	}
	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);

	//read thread나 write thread가 활성화 되어 있는 경우 대기
	if(rw->read_cnt > 0 || rw->write_cnt >0){
		//대기중인 write thread가 존재하는지 확인하기 위하여
		//write_wait 증가
		rw->write_wait++;
		pthread_cond_wait(&rw->w_cond, &rw->rw_mutex);
		//대기 종료 시 감소
		rw->write_wait--;
	}
	rw->write_cnt++;
	pthread_mutex_unlock(&rw->rw_mutex);
}

void w_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->rw_mutex);
	rw->write_cnt--;
	
	//대기중인 write thread가 있을 경우
	//대기중인 write thread를 깨워줌
	if(rw->write_wait > 0){
		pthread_cond_signal(&rw->w_cond);
	}
	//대기중인 write thread가 없을 경우
	//모든 대기중인 read thread를 깨워준다.
	else if(rw->write_wait ==0){
		pthread_cond_broadcast(&rw->r_cond);
	}
	pthread_mutex_unlock(&rw->rw_mutex);

}
