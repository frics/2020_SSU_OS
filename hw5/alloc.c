#include "alloc.h"
//page size : 4096 -> 4KB
//min alloc : 8
#define SIZE 512

struct mm{
	struct mm* next;
	int size;
	int is_used;
};

struct mm mem[SIZE];
int init_alloc(){
	for(int i=0; i<SIZE; i++){
		mem[i].size = 8;
		mem[i].is_used = 0;
	}
	mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	return 0;
}

int cleanup(){
	if(munmap(0, PAGESIZE))
		return 1;
	return 0;
}
char *alloc(int n){

	int size = n/MINALLOC;
	printf("size : %d\n", size);
//	char addr;

//	return addr;

}

void dealloc(char *adr){
	printf("deallocing...\n");
	return;
}




















































