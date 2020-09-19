#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include<wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define INPUT 0
#define OUTPUT 1

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
/*
	//토큰 디버깅
	if(flag==0)
		printf("\nCheck Tokens : ");
	else
		printf("\nCheck Pipes : ");
	for(i=0; tokens[i]!=NULL; i++){
		printf("(%s)", tokens[i]);
	}
	printf("\n");
	*/
	return tokens;
}


void freeToken(char **tokens){
	for(int i =0; tokens[i]!= NULL; i++){
		//	free(tokens[i]);
		tokens[i] = '\0';
	}
	free(tokens);
}
int main(int argc, char *argv[]) {
	int cmd_cnt, pipe_cnt;
	char line[MAX_INPUT_SIZE];
	char **tokens;
	char **commands, **command;
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
		//공백일 경우 넘어간다.
		if(strcmp(line, "")==0)
			continue;

		//PIPE Check
		for(i=0; i<strlen(line); i++){
			if(line[i] == '|')
				cmd_cnt++;
		}
		pipe_cnt = cmd_cnt-1;
		printf("**********************\nCommands : %d\nPipes :%d\n**********\n", cmd_cnt, pipe_cnt);

		//commands : '|' 별로 잘린 명령어 토큰
		//command : 잘린 명령어 실행을 위해 선언
		commands = tokenize(line, 1);

		int fds[pipe_cnt][2];

		/*파이프 전체 생성
		  for(i=0; i<(2*pipe_cnt); i+=2){
		  if(pipe(fds+i) == -1)
		  printf("파이프 생성 실패\n");
		  }*/

		int p=0;
		//명령어 개수만큼 반복문 돌려서 실행
		for(i=0; i<cmd_cnt; i++){
			command = tokenize(commands[i], 0);
			if(pipe_cnt>0 && pipe(fds[i]))
				printf("파이프 생성 실패\n");

			//명령어 만큼 프로세스 생성
			pid = fork();
		

			//명령어 실행을 위해 i번째 명령어 실행

			switch(pid){
				case -1:
					printf("FORK FAIL\n");
					break;
				case 0 :
					//첫번째 파이프
					if(cmd_cnt > 1){
						if(i == 0){
							printf("Is first cmd\n");
							close(fds[i][INPUT]);
							dup2(fds[i][OUTPUT], STDOUT_FILENO);
						}
						// 마지막 파이프
						else if(i == pipe_cnt){
							printf("Is last pipe cmd\n");
							close(fds[i][OUTPUT]);
							dup2(fds[i][INPUT], STDIN_FILENO);
						}
						//중간 파이프
						else{
							printf("middle pipe\n");
							dup2(fds[i][INPUT], STDIN_FILENO);
							dup2(fds[i][OUTPUT], STDOUT_FILENO);
						}
					}

			
					printf("pid(%d) : %s %s\n", getpid(), command[0], command[1]);
					execvp(command[0], command);
					printf("잘못된 명령어입니다.\n");
					exit(9);
					break;

				default:
					for(j=0; j<cmd_cnt; j++){
						printf("waiting..... pid : %d\n", getpid());
						wait(&status);
						close(fds[j][OUTPUT]);
						close(fds[j][INPUT]);
						if(status == -1)
							printf("fuck u\n");
					}
					break;
			}

		}
		
		freeToken(command);
		freeToken(commands);
	}
	return 0;
}
