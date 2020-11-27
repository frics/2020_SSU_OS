#include "ealloc.h"
#define NUM 16
#define PNUM 4

struct mm{
	char *adr;
	int size;
	int flag;
};

//총 4개의 PAGE의 시작주소를 저장할 배열 생성
char *start[PNUM];
//memory manager 구조체를 
//PAGE NUM * MAX CHUNK 만큼 생성
struct mm mem[PNUM][NUM];

void init_alloc(){
	//memory manager 구조체 초기화
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
	//memory manager 구조체 초기화
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

	if(size > PAGESIZE || size%8 != 0)
		return NULL;

	//First Fit 알고리즘 적용
	//page 순서대로 탐색
	for(int p=0; p<PNUM; p++){

		//p-1번째 페이지에 free한 공간이 없을때
		//다음으로 탐색할 p번째 페이지가 없으면 생성
		if(start[p] == NULL){
			start[p] = (char *)mmap(0, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			if(start[p] == MAP_FAILED)
				return NULL;
		}
		//p번째 테이블에서 free한 memory 탐색
		for(int i = 0; i < NUM; i++){
			valid = 0;

			if(mem[p][i].flag ==0){

				for(int j=i; j<NUM; j++){
					//탐색중 적정 size 도달전
					//사용중인 memory가 있으면 break;
					if(mem[p][j].flag == 1)
						break;

					valid += MINALLOC;
					//fit한 메모리 공간 탐색 성공
					//해당하는 memory manager에 정보 저장
					if(size <= valid){
						mem[p][i].size = valid;
						mem[p][i].flag = 1;
						adr = i*MINALLOC+start[p];
						mem[p][i].adr = adr;
						return adr;
					}
				}
			}
			//사용중인 메모리 공간을 발견하고
			//memory manager의 size가 8이상일 경우
			else if(mem[p][i].size != MINALLOC){
				//해당하는 메모리가 사용중인 인덱스만큼 jump시킴
				i += mem[p][i].size/MINALLOC-1;
				if(i>=NUM)
					break;
			}
		}
	}

	return NULL;
}

void dealloc(char *adr){

	//adr에 해당하는 메모리 공간을 찾아서
	//메모리 해제
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
