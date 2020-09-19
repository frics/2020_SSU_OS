#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<wait.h>
#include<string.h>

int main()
{
	char buf[256];
	char *arg[7];
	char *s;
	char *save;
	static const char delim[] = "\t\n";
	printf("$ : ");
	scanf("%[^\n]", buf);
	getchar();
	s = strtok_r(buf, delim, &save);
	int argv = 0;
	while(s){
		arg[argv++] = s;
		printf("|%s| \t", s);
		s = strtok_r(NULL, delim, &save);
	}
	printf("\n");
	arg[argv] = (char *)0;

	//char *arg[] ={"ls",  (char*)0};
	pid_t pid;
	int status;
	pid = fork();

	switch(pid){
		case -1 :
			printf("fork failed");
			break;
		case 0 :
			execvp(arg[0],arg);
			printf("명령어 실행 실패\n");
			break;
		defualt :
			pid = wait(&status);
			printf("부모 프로세스 대기중\n");
			break;
	}
	return 0;
}
