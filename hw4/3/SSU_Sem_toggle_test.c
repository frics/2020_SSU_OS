#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"
#include <unistd.h>

#define NUM_THREADS 3
#define NUM_ITER 10

//쓰레드들 간의 동기화를 위한 SSU_Sem 선언
SSU_Sem child;
SSU_Sem parent;
void *justprint(void *data)
{
	int thread_id = *((int *)data);
	//대기 상태에 들어간 main thread를 깨워서
	//나머지 쓰레드들을 생성하게 한다.
	SSU_Sem_up(&parent);
	for(int i=0; i < NUM_ITER; i++)
	{
		//child thread를 up 시키키전까지 대기 상태에 놓는다.
		//순서대로 출력하기 위함.
		SSU_Sem_down(&child);
		printf("This is thread %d\n", thread_id);
		//출력 완료 후 main thread를 깨워
		//다음 child thread를 깨워 출력하게 한다.
		SSU_Sem_up(&parent);
	}
	return 0;
}

int main(int argc, char *argv[])
{

	pthread_t mythreads[NUM_THREADS];
	int mythread_id[NUM_THREADS];

	//child, parent SSU_Sem 초기화
	SSU_Sem_init(&child, 0);
	SSU_Sem_init(&parent, 0);

	for(int i =0; i < NUM_THREADS; i++)
	{
		mythread_id[i] = i;
		pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
		//생성 순서대로 wait큐에 들어가게 하기 위해서
		//thread 생성 후 main thread를 대기 상태에 놓는다.
		SSU_Sem_down(&parent);
	}
	
	//thread의 개수 * 반복 횟수 만큼
	//child thread와 main thread를
	//실행 시키고 대기 시키는것을 반복한다.
	for(int i=0; i<NUM_ITER*NUM_THREADS; i++){
		SSU_Sem_up(&child);
		SSU_Sem_down(&parent);
	}

	for(int i =0; i < NUM_THREADS; i++)
	{
		pthread_join(mythreads[i], NULL);
	}

	return 0;
}
