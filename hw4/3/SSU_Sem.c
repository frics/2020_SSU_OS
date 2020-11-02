#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
	pthread_mutex_init(&s->mutex, NULL);
	pthread_cond_init(&s->cond, NULL);
	s->value = value;
	return;
}

void SSU_Sem_down(SSU_Sem *s) {

	pthread_mutex_lock(&s->mutex);
	s->value -= 1;
	if(s->value <0){
		pthread_cond_wait(&s->cond, &s->mutex);
	}
	pthread_mutex_unlock(&s->mutex);
	return;
}

void SSU_Sem_up(SSU_Sem *s) {
	pthread_mutex_lock(&s->mutex);

	s->value += 1;
	if(s->value <= 0){
		pthread_cond_signal(&s->cond);
	}

	pthread_mutex_unlock(&s->mutex);
	return;
}
