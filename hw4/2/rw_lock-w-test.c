#include "rw_lock.h"

void init_rwlock(struct rw_lock * rw)
{
	//읽기
	pthread_mutex_init(&rw->r_con_mutex, NULL);
	pthread_cond_init(&rw->r_con, NULL);
	//쓰기
	pthread_mutex_init(&rw->w_con_mutex, NULL);
	pthread_cond_init(&rw->w_con, NULL);
	pthread_mutex_init(&rw->w_mutex, NULL);

	rw->read_cnt = 0;
	rw->write_cnt = 0;

	return;
}

void r_lock(struct rw_lock * rw)
{
	//누군가 쓰고 있는지 확인
	/*
	pthread_mutex_lock(&rw->w_con_mutex);
	if(rw->write_cnt > 0)
		pthread_cond_wait(&rw->w_con, &rw->w_con_mutex);
	pthread_mutex_unlock(&rw->w_con_mutex);
*/
	//read lock은 누구나 획득 가능
	pthread_mutex_lock(&rw->r_con_mutex);
	rw->read_cnt++;
	pthread_mutex_unlock(&rw->r_con_mutex);
}

void r_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->r_con_mutex);
	rw->read_cnt--;
	//아무도 read하고 있지 않을때
	//write_thread 활성화
	//아닐수도 ㅋㅋ
	if(rw->read_cnt == 0){
		//broadcast 써보는것도 ㄱㅊ 
		pthread_cond_signal(&rw->r_con);
	}

	pthread_mutex_unlock(&rw->r_con_mutex);
}

void w_lock(struct rw_lock * rw)
{
	//write 하려면 read, write 둘다 획득해야한다.
	//write lock 획득 -> write_cnt 변경
	pthread_mutex_lock(&rw->w_con_mutex);
	rw->write_cnt++;
	pthread_mutex_unlock(&rw->w_con_mutex);
	//read lock 획득

	pthread_mutex_lock(&rw->r_con_mutex);
	//누군가 읽고 있다면 waiting
	if(rw->read_cnt > 0){
		pthread_cond_wait(&rw->r_con, &rw->r_con_mutex);
	}
	pthread_mutex_unlock(&rw->r_con_mutex);

	//최종적으로 write mutex 획득

	pthread_mutex_lock(&rw->w_mutex);
}

void w_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->w_con_mutex);
	rw -> write_cnt--;

	if(rw->write_cnt == 0)
		pthread_cond_signal(&rw->w_con);
	pthread_mutex_unlock(&rw->w_con_mutex);

	pthread_mutex_unlock(&rw->w_mutex);

}
