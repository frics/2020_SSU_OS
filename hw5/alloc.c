#include "alloc.h"
#define NUM 512

struct mm{
	char *adr;
	int size;
	int flag;
};
char* start;
struct mm mem[NUM];
int init_alloc(){
	for(int i=0; i<NUM; i++){
		mem[i].adr = 0;
		mem[i].size = MINALLOC;
		mem[i].flag= 0;
	}
	start = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	return 0;
}
int cleanup(){
	if(munmap(0, PAGESIZE))
		return 1;
	return 0;
}
char *alloc(int size){
	char* adr;

	for(int i = 0 ; i < NUM; i++){
		int valid =0;
		if(mem[i].flag == 0){
			for(int j=i; mem[j].flag != 1 || j<NUM; j++){
				valid += MINALLOC;
				if(size <= valid){
					mem[i].size = valid;
					mem[i].flag = 1;
					adr = i*MINALLOC+start;
					mem[i].adr = adr;
					return adr;
				}
			}
		}else if(mem[i].size != MINALLOC){
			i += (mem[i].size/MINALLOC -1);
			if(i>=NUM)
				break;
		}
	}
	return NULL;
}
void dealloc(char *adr){
	for(int i = 0; i<NUM; i++){
		if(mem[i].adr == adr){
			mem[i].adr = 0;
			mem[i].size = MINALLOC;
			mem[i].flag = 0;
			break;
		}
	}
	return;
}
