#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
	//mutex, cond 초기화
	pthread_mutex_init(&s->mutex, NULL);
	pthread_cond_init(&s->cond, NULL);
	s->value = value;
	return;
}

void SSU_Sem_down(SSU_Sem *s) {

	pthread_mutex_lock(&s->mutex);
	//semaphore value 감소
	s->value -= 1;
	//value가 음수값을 내려가면 해당 thread 대기
	if(s->value <0){
		pthread_cond_wait(&s->cond, &s->mutex);
	}
	pthread_mutex_unlock(&s->mutex);
	return;
}

void SSU_Sem_up(SSU_Sem *s) {
	pthread_mutex_lock(&s->mutex);

	//semaphore value 증가
	s->value += 1;
	//value가 0보다 작거나 같으면 wait큐에 들어있는 변수를 깨움
	if(s->value <= 0){
		pthread_cond_signal(&s->cond);
	}

	pthread_mutex_unlock(&s->mutex);
	return;
}
