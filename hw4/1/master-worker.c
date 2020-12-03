#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
int item_to_produce, curr_buf_size, consume_cnt;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;
//mutex, cond의 선언과 초기화
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t consume = PTHREAD_COND_INITIALIZER;


void print_produced(int num, int master) {

	printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

	printf("Consumed %d by worker %d\n", num, worker);

}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
	int thread_id = *((int *)data);

	while(1)
	{
		//mutex lock 획득 
		pthread_mutex_lock(&mutex);
		
		//curr_buf_size가 꽉찼을때
		//consume이 되서 버퍼에 공간이 생길때까지 대기
		if(curr_buf_size >= max_buf_size){
			pthread_cond_wait(&produce, &mutex);
		}
		//사용자가 입력한 만큼의 숫자가 생성 완료되면
		//master thread를 종료하고
		//pthread_cond_broadcast를 통하여
		//자고 있는 나머지 master thread들을 깨워 종료시킨다.  
		if(item_to_produce >= total_items) {
			pthread_cond_broadcast(&produce);
			pthread_mutex_unlock(&mutex);
			return 0;
		}

		buffer[curr_buf_size++] = item_to_produce;
		print_produced(item_to_produce, thread_id);
		item_to_produce++;

		//item이 생성되었다고 알림
		pthread_cond_signal(&consume);
		//mutex lock 해제
		pthread_mutex_unlock(&mutex);

	}
	return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

void *consume_requests_loop(void *data)
{
	int thread_id = *((int *)data);
	int item_to_consume;
	while(1)
	{
		pthread_mutex_lock(&mutex);

		//curr_buf_size를 다 소비했을때
		//produce에서 item을 추가로 생성해줄때까지 대기
		if(curr_buf_size <= 0 && consume_cnt < total_items){
			pthread_cond_wait(&consume, &mutex);
		}

		//사용자가 입력한 만큼의 숫자가 소비 완료되면
		//worker thread를 종료하고
		//pthread_cond_broadcast를 통하여
		//자고 있는 나머지 worker thread를 깨워 종료시킨다.  
		if(consume_cnt == total_items){
			pthread_cond_broadcast(&consume);
			pthread_mutex_unlock(&mutex);
			return 0;
		}

		//curr_buf_size가 0이하인 상태로
		//wait에서 해제된 thread가 음수 인덱스에 접근하는것을 
		//막기 위한 예외처리
		if(curr_buf_size <=0)
			pthread_cond_wait(&consume, &mutex);
		else{
			curr_buf_size--;
			item_to_consume = buffer[curr_buf_size];
			buffer[curr_buf_size] = -1;
			consume_cnt++;
			print_consumed(item_to_consume, thread_id);
		}
		//item이 소비될때 produce로 신호를 보냄
		pthread_cond_signal(&produce);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int *master_thread_id;
	int *worker_thread_id;
	pthread_t * worker_thread;
	pthread_t *master_thread;
	item_to_produce = 0;
	curr_buf_size = 0;

	int i;


	if (argc < 5) {
		printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
		exit(1);
	}
	else {
		num_masters = atoi(argv[4]);
		num_workers = atoi(argv[3]);
		total_items = atoi(argv[1]);
		max_buf_size = atoi(argv[2]);
	}

	buffer = (int *)malloc (sizeof(int) * max_buf_size);

	//create master producer threads
	master_thread_id = (int *)malloc(sizeof(int) * num_masters);
	master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
	for (i = 0; i < num_masters; i++)
		master_thread_id[i] = i;

	for (i = 0; i < num_masters; i++)
		pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

	//create worker consumer threads

	worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
	worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);

	for (i = 0; i < num_workers; i++)
		worker_thread_id[i] = i;

	for (i = 0; i < num_workers; i++)
		pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);


	//wait for all threads to complete
	for (i = 0; i < num_masters; i++)
	{
		pthread_join(master_thread[i], NULL);
		printf("master %d joined\n", i);
	}
	for (i = 0; i < num_workers; i++)
	{
		pthread_join(worker_thread[i], NULL);
		printf("worker %d joined\n", i);
	}

	/*----Deallocating Buffers------------------*/
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&produce);
	pthread_cond_destroy(&consume);
	free(buffer);
	// memset(master_thread_id, 0, sizeof(master_thread_id));
	free(master_thread_id);

	// memset(worker_thread_id, 0, sizeof(master_thread_id));
	free(worker_thread_id);
	
	free(master_thread);
	free(worker_thread);

	return 0;
}
