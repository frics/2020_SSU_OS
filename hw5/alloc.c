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

	cur_position = 0;
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

	for(int i = 0 ; i < NUM; i++)
	{
		int valid =0;
		if(mem[i].flag == 0)
		{
			valid += MINALLOC;
			if(size <= valid)
			{
				//여기는 안들어가도 되긴함
				//mem[i].size = valid;
				//printf("find!!\n");
				mem[i].flag = 1;
				adr = i*MINALLOC+start;
				return adr;
			}else
			{
//				printf("메모리 확장중\n");
				int j = i+1;
				while(mem[j].flag != 1 || j<NUM){
					valid += MINALLOC;
					if(size <= valid)
					{
						//printf("find!!\n");
						//printf("현재 블럭 크기: %d\n", valid);
						//printf("index : %d\n", i);
						mem[i].size = valid;
						mem[i].flag = 1;
						adr = i*MINALLOC+start;
						mem[i].adr = adr;
						return adr;
					}
					j++;
				}
			}
		}else if(mem[i].size != MINALLOC){
			//printf("index jumping\n");
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
//			printf("해제할거 찾음\n");
			mem[i].adr = 0;
			mem[i].size = MINALLOC;
			mem[i].flag = 0;
			break;
		}
	}
	return;
}

