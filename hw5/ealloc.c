#include "ealloc.h"
#define NUM 16
#define PNUM 4

struct mm{
	char *adr;
	int size;
	int flag;
};
char* start[PNUM];
struct mm mem[PNUM][NUM];

void init_alloc(){
	for(int p=0; p <PNUM; p++){ 
		start[p] = NULL;
		for(int i=0; i<NUM; i++){
			mem[p][i].adr = NULL;
			mem[p][i].size = MINALLOC;
			mem[p][i].flag= 0;
		}
	}
	//	start = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	return;
}
void cleanup(){
//	system("ps -u | grep ./test_ealloc");
//	printf("\n\n\n\n");
	for(int i=0; i<PNUM; i++){
		if(start[i] != NULL)
			munmap(start[i], PAGESIZE);
	}
//	system("ps -u | grep ./test_ealloc");
	return;
}
char *alloc(int size){
	char* adr;

	for(int p=0; p<PNUM; p++){
		if(start[p] == NULL){
			start[p] = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			printf("%d번째 페이지 생성\n", p+1);
		}
		for(int i = 0; i < NUM; i++){
			int valid = 0;
			if(mem[p][i].flag ==0){
				for(int j=0; mem[p][j].flag != 1 || j<NUM; j++){
					valid += MINALLOC;
					if(size <= valid){
						mem[p][i].size = valid;
						mem[p][i].flag = 1;
						adr = i*MINALLOC+start[p];
						mem[p][i].adr = adr;
						return adr;
					}
				}
			}else if(mem[p][i].size != MINALLOC){
				i +=(mem[p][i].size/MINALLOC-1);
				if(i>=NUM)
					break;
			}
		}
	}
	return NULL;
}
void dealloc(char *adr){
	for(int p = 0; p<PNUM; p++){
		for(int i = 0; i<NUM; i++){
			if(mem[p][i].adr == adr){
				mem[p][i].adr = 0;
				mem[p][i].size = MINALLOC;
				mem[p][i].flag = 0;
				break;
			}
		}
	}
	return;
}
