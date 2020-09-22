#include<stdio.h>
#include<time.h>



int main(){

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	int total=0;
	int running =0;
	int sleeping =0;
	int stopped =0;
	int zombie = 0;
	printf("top - %d:%d:%d up\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
	//Tasks
	printf("Task: %4d total,", total);
	printf("%4d running,", running);
	printf("%4d sleeping,", sleeping);
	printf("%4d stopped,", stopped);
	printf("%4d zombie\n", zombie);
	//%Cpu(s)
	printf("%%Cpu(s):%5.1f us,",0.1);
	printf("%5.1f sy,", 1.0);
	printf("%5.1f ni,", 1.0);
	printf("%5.1f id,", 99.0);
	printf("%5.1f wa,", 0.0);
	printf("%5.1f hi,", 0.0);
	printf("%5.1f si,", 0.0);
	printf("%5.1f st\n", 0.0);


	
	
}
