#include "alloc.h"
#define NUM 512

//memory manager 구조체 선언
struct mm{
	char *adr;
	int size;
	int flag;
};
//Memory page 시작주소
char *start;
//mm구조체 배열 선언
struct mm mem[NUM];

int init_alloc(){
	//memory manager 구조체 배열 초기화
	for(int i=0; i<NUM; i++){
		mem[i].adr = NULL;
		mem[i].size = MINALLOC;
		mem[i].flag= 0;
	}
	//4KB만큼 memory mapping
	start = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	
	if(start == MAP_FAILED)
		return 1;

	return 0;
}

int cleanup(){
	//mmap을 통해 할당한 memory 해제
	if(munmap(start, PAGESIZE)==-1)
		return 1;
	return 0;
}

char *alloc(int size){

	char *adr;
	int valid;

	if(size > PAGESIZE || size % 8 != 0)
		return NULL;

	//First fit 알고리즘 적용
	for(int i = 0 ; i < NUM; i++){
		valid =0;
		
		if(mem[i].flag == 0){

			for(int j=i; j<NUM; j++){
				//탐색중 적정 size 도달전
				//사용중인 memory가 있으면 break
				if(mem[j].flag ==1)
					break;

				valid += MINALLOC;
				//fit한 메모리 탐색 성공
				//해당하는 memory manager에 정보 저장
				if(size <= valid){
					mem[i].size = valid;
					mem[i].flag = 1;
					//return할 adr 저장
					adr = i*MINALLOC+start;
					mem[i].adr = adr;
					return adr;
				}
			}
		}
		//사용중인 메모리 공간을 발견하고
		//memory manager의 size가 8이상일 경우
		else if(mem[i].size != MINALLOC){
			//해당 메모리가 사용중인 공간만큼 인덱스 jump
			i += mem[i].size/MINALLOC -1;
			if(i>=NUM)
				break;
		}
	}

	return NULL;
}

void dealloc(char *adr){

	//adr에 해당하는 메모리 공간을 찾아서
	//메모리 해제
	for(int i = 0; i<NUM; i++){
		if(mem[i].adr == adr){
			mem[i].adr = NULL;
			mem[i].size = MINALLOC;
			mem[i].flag = 0;
			break;
		}
	}
	return;
}
