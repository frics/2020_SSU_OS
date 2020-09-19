#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include<wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define WRITE 1

/* Splits the string by space and returns the array of tokens
   flag = 0 : 문자열을 공백으로 구분 -> 명령어 단어 별로 토큰화
   flag = 1 : 문자열을 파이프로 구분 -> 명령어 별로 토큰화
 ********************/
char **tokenize(char *line, int flag) {
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;
	char pipe_ch;

	//개행 처리로 구분해줌
	line[strlen(line)] = '\n';

	if(flag==0)
		pipe_ch = ' ';
	else
		pipe_ch = '|';

	for (i = 0; i < strlen(line); i++) {

		char readChar = line[i];

		if (readChar == pipe_ch || readChar == '\n' || readChar == '\t') {
			token[tokenIndex] = '\0';
			if (tokenIndex != 0) {
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		} else
			token[tokenIndex++] = readChar;
	}


	free(token);
	tokens[tokenNo] = NULL;

	//토큰 디버깅
	
	printf("Check Tokens : ");
	for(i=0; tokens[i]!=NULL; i++){
		printf("(%s)", tokens[i]);
	}
	printf("\n");
	return tokens;
}


void freeToken(char **tokens){
	for(int i =0; tokens[i]!= NULL; i++)
		free(tokens[i]);
	free(tokens);
}
int main(int argc, char *argv[]) {
	int cmd_cnt;
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i, j;
	int status;
	pid_t pid;

	FILE *fp;
	if (argc == 2) {
		fp = fopen(argv[1], "r");
		if (fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while (1) {
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		cmd_cnt=1;

		//배치 모드일 경우
		if (argc == 2) {                               // batch mode
			if (fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;
			}
			line[strlen(line) - 1] = '\0';
		}
		//반응형 모드
		else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		//PIPE Check
		for(i=0; i<strlen(line); i++){
			if(line[i] == '|')
				cmd_cnt++;
		}
		//		if(cmd_cnt>1){
		printf("요거슨 파이프구여\nCommands : %d\nPipes :%d\n", cmd_cnt, cmd_cnt-1);
		int pipe_cnt = cmd_cnt-1;
		//commands : '|' 별로 잘린 명령어 토큰
		//command : 잘린 명령어 실행을 위해 선언
		char **commands, **command;
		commands = tokenize(line, 1);
		int fds[2*pipe_cnt];
		//파이프 전체 생성
		for(i=0; i<(2*pipe_cnt); i+=2){
			if(pipe(fds+i) == -1)
				printf("파이프 생성 실패\n");
		}
		
			int p=0;
		//명령어 개수만큼 반복문 돌려서 실행
		for(i=0; i<cmd_cnt; i++){

			//명령어 실행을 위해 i번째 명령어 실행
			command = tokenize(commands[i], 0);

			//파이프 한번 거칠때마다 프로세스 생성
			pid = fork();
			switch(pid){
				case 0 :
					//printf("-------------------\n");
					//printf("ppid : %d\npid : %d\ncommand : %s %s\n", getppid(), getpid(), command[0], command[1]);
					//첫번째 명령어 일때
					if(i == 0){//출력 파이프
						printf("fuck%d번째 명령어\n", i);
						dup2(fds[1], 1);
					}
					// 마지막 파이프
					else if(i == pipe_cnt){
						printf("sibal%d번째 명령어\n", i);
						dup2(fds[2*(i-1)], 0);
					//그 외 다른 파이프
					}else{
						printf("sex%d번째 명령어\n", i);
						dup2(fds[p],0);
						p+=2;
						dup2(fds[p+1],1);
					}

					for(i=0; i<(2*pipe_cnt); i+=2){
						if(close(fds[i]) == -1 || close(fds[i+1]) == -1)
							printf("pipe close fail\n");
					}
					printf("이제 이거 실행 : %s %s\n", command[0], command[1]);
					if(execvp(command[0], command)<0)
						printf("잘못된 명령어입니다.\n");
					break;
				case -1:
					printf("FORK FAIL\n");
					break;

				default:
					wait(&status);
					exit(0);
					break;
			}
		}
		for(i=0; i<(2*pipe_cnt); i+=2){
			if(close(fds[i]) == -1 || close(fds[i+1]) == -1)
				printf("pipe close fail\n");
		}
		
		/*for(i=0; i<cmd_cnt; i++){
	//		printf("pid : %d\n", getpid());
			wait(&status);
			//printf("pid :%d\n", getpid());
		}
		*/
		freeToken(command);
	    freeToken(commands);

		/*		}
				else{
		//자식 프로세스 생성
		//pid_t pid;
		pid = fork();

		tokens = tokenize(line, 0);
		switch(pid){
		case -1://fork fail
		printf("fork 실패\n프로그램 종료");
		break;
		case 0://fork suc

		printf("parent(In ori) : %d\n", getppid());
		printf("child(In ori) : %d\n", getpid());


		//PIPE Check here?
		if(execvp(tokens[0],tokens)<0){
		printf("잘못된 명령어입니다.\n");
		}
		return 5;
		default://not child process
		//자식 프로세스가 끝날때까지
		//부모 프로세스 대기
		wait(NULL);
		break;
		}
		//토큰에 할당된 메모리 해제
		freeToken(tokens);

		}*/
	}
	return 0;
}
