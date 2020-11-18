#include "ealloc.h"
#define NUM 16
#define PNUM 4

struct mm{
	char *adr;
	int size;
	int flag;
};

char *start[PNUM];
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

	return;
}

void cleanup(){
	for(int p=0; p <PNUM; p++){ 
		start[p] = NULL;

		for(int i=0; i<NUM; i++){
			mem[p][i].adr = NULL;
			mem[p][i].size = MINALLOC;
			mem[p][i].flag= 0;
		}
	}
	return;
}

char *alloc(int size){
	char *adr;
	int valid;
	for(int p=0; p<PNUM; p++){

		if(start[p] == NULL)
			start[p] = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

		for(int i = 0; i < NUM; i++){
			valid = 0;

			if(mem[p][i].flag ==0){

				for(int j=i; j<NUM; j++){

					if(mem[p][j].flag == 1)
						break;

					valid += MINALLOC;
					if(size <= valid){
						mem[p][i].size = valid;
						mem[p][i].flag = 1;
						adr = i*MINALLOC+start[p];
						mem[p][i].adr = adr;
						return adr;
					}
				}
			}
			else if(mem[p][i].size != MINALLOC){
				i += mem[p][i].size/MINALLOC-1;
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
				mem[p][i].adr = NULL;
				mem[p][i].size = MINALLOC;
				mem[p][i].flag = 0;
				break;
			}
		}
	}
	return;
}
