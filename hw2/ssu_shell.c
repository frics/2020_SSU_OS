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
		tokens[i] = '\0';
	}
	free(tokens);
}
void executeCommand(char **commands, int cmd_cnt){

	int fd[2];
	int fd_next;
	char **command;
	for(int i=0; i<cmd_cnt; i++){
		if(cmd_cnt>1)
			pipe(fd);
		command = tokenize(commands[i], 0);
		switch(fork()){
			case 1:
				printf("FORK FAIL\n");
				return;
			case 0:
				if(cmd_cnt>1){
					//prev fd를 복사해줌
					dup2(fd_next, 0);
					//다음 명령어가 없을때
					if(commands[i+1] != NULL)
						dup2(fd[1], 1);
					close(fd[0]);
					close(fd[1]);
				}
				execvp(command[0], command);
				printf("잘못된 명령어입니다.\n");
				exit(1);
				break;
			default:
				if(cmd_cnt>1){
					close(fd[1]);
					fd_next = fd[0];
				}
				break;
		}
	}
	while(wait(NULL)>0);
}
int main(int argc, char *argv[]) {
	int cmd_cnt;
	char line[MAX_INPUT_SIZE];
	char **tokens;
	char **commands;
	int i;

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

		//batch mode일 경우
		if (argc == 2) {
			if (fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;
			}
			line[strlen(line) - 1] = '\0';
		}
		//interactivie mode일 경우
		else {
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		//공백일 경우 넘어간다.
		if(strcmp(line, "")==0)
			continue;

		//Command Count Check
		//'|' 기준으로 명령어 개수 카운트
		for(i=0; i<strlen(line); i++){
			if(line[i] == '|')
				cmd_cnt++;
		}

		//commands : '|' 별로 잘린 명령어 토큰
		tokens = tokenize(line, 1);
		executeCommand(tokens, cmd_cnt);
		freeToken(tokens);
	}
	return 0;
}
