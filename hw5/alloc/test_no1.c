#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"

int main()
{
	//mmap to get page
	if(init_alloc())
		return 1;	//mmap failed
	//make test case
	char *strOverSize = alloc(4096*2);
	if(strOverSize == NULL)
		printf("Oversize Test passed\n");
	else
		printf("Oversize Test failed\n");

	char *strWrong = alloc(7);
	if(strWrong == NULL)
		printf("Wrongsize Test passed\n");
	else
		printf("Wrongsize Test failed\n");

	
	printf("Starting comprehensive tests (see details in code)\n");

	/*
	Comprehensive tests:
	1. Populate entire thing in blocks of for loop (a's, b's, c's, and d's) equal size.
	2. Dealloc c's, reallocate and replace with x's.
	3. 
	*/
	/*** test 1 ****/
	
	//Generating ideal strings for comparison
	char stringA[1024], stringB[1024], stringC[1024], stringD[1024], stringX[1024];
	for(int i = 0; i < 1023; i++)
	{
		stringA[i] = 'a';
		stringB[i] = 'b';
		stringC[i] = 'c';
		stringD[i] = 'd';
		stringX[i] = 'x';
	}

	stringA[1023] = stringB[1023] = stringC[1023] = stringD[1023] = stringX[1023] = '\0';

	char *strA = alloc(1024);
	char *strB = alloc(1024);
	char *strC = alloc(1024);
	char *strD = alloc(1024);

	for(int i = 0; i < 1023; i++)
	{
		strA[i] = 'a';
		strB[i] = 'b';
		strC[i] = 'c';
		strD[i] = 'd';
	}
	strA[1023] = strB[1023] = strC[1023] = strD[1023] = '\0';

	if(strcmp(stringA, strA) == 0 && strcmp(stringB, strB) == 0 && strcmp(stringC, strC) == 0 && strcmp(stringD, strD) == 0)
	  printf("Test 1 passed: allocated 4 frees of 1KB each\n");
	else
	  printf("Test 1 failed: A: %d, B: %d, C: %d, D: %d\n", strcmp(stringA, strA), strcmp(stringB, strB), strcmp(stringC, strC), strcmp(stringD, strD));


	// merge checks
	//test 4: free 2x512, allocate 1024
	dealloc(strB);
	dealloc(strD);
	char *strX = alloc(1024);
	for(int i = 0; i < 1023; i++)
	{
		strX[i] = 'b';
	}
	strX[1023] = '\0';

	if(strcmp(stringB, strX) == 0)
	  printf("Test 4 passed: merge worked\n");
	else
	  printf("Test 4 failed: X: %d\n", strcmp(stringB, strX));

	dealloc(strX);
	
	char *strY = alloc(2048);
	if(strY == NULL)
		printf("Mapping Test passed\n");
	else
		printf("Mapping Test failed\n");

	dealloc(strC);
	strX = alloc(3072);
	char  stringM[3072];
	for (int i=0;i<3071;i++){
		strX[i]=stringM[i]='z';
	}
	strX[3071]=stringM[3071]='\0';
	if (!strcmp(stringM, strX))
		printf("Test 5 passed: merge alloc 3072 worked\n");
	else
		printf("Test5 failed\n");
	///////////////////////////

	if(cleanup())
		return 1;	//munmap failed
	return 0;
}
