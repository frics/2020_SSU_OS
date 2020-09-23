#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 ********************/
char **tokenize(char *line, int flag) {
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;
	char pipe_ch;

	//명령어 개행 처리
	line[strlen(line)] = '\n';

	//flag가 0이면 명령어 토큰화
	//flag가 1이면 파이프 기준으로 토큰화
	if(flag==0)
		pipe_ch = ' ';
	else
		pipe_ch = '|';

	for (i = 0; i < strlen(line); i++) {

		char readChar = line[i];
		//파이프 토큰 or 명령어 토큰 분기점
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
	return tokens;
}

//사용완료된 토큰 해제 함수
void freeToken(char **tokens){
	for(int i =0; tokens[i]!= NULL; i++){
		tokens[i] = '\0';
	}
	free(tokens);
}
//명령어 실행 함수
//명령어 갯수(cmd_cnt)와 입력된 명령어(commands)를 매개변수로 받아온다.
void executeCommand(char **commands, int cmd_cnt){
	//file descriptor 변수
	int fd[2];
	//
	int fd_prev = 0;
	//명령어를 토큰화하여 저장할 변수 선언
	char **command;
	for(int i=0; i<cmd_cnt; i++){
		//파이프 생성
		if(cmd_cnt>1)
			pipe(fd);
		//파이프 기준으로 토큰화된 명령어들 중에
		//i번째 명령어를 토큰화하여 command에 저장
		command = tokenize(commands[i], 0);

		switch(fork()){
			case 1:						//fork()실패
				printf("FORK FAIL\n");
				return;
			case 0:						//fork()성공
				//cmd_cnt가 1이상일때만 파이프 연결
				if(cmd_cnt>1){
					//INPUT을 전 명령어와 연결
					dup2(fd_prev, 0);
					//마지막 명령어가 아닐때
					//dup2()로 출력을 연결해준다.
					if(commands[i+1] != NULL)
						dup2(fd[1], 1);
					close(fd[0]);
				}
				//명령어 실행
				execvp(command[0], command);
				//명령어 실행 실패 시 프린트문 출력
				printf("SSUSHELL : Incorrect command\n");
				exit(1);
				break;
			default:				//부모 프로세스
				//cmd_cnt가 1 이상일때만 수행
				if(cmd_cnt>1){
					close(fd[1]);
					//파이프 연결을 위한 변수 임시 저장
					fd_prev = fd[0];
				}
				break;
		}
	}
	//자식 프로세스들이 종료될때까지 대기
	while(wait(NULL)>0);
	//생성되어 있는 모든 파이프를 닫아준다. 
	for(int i=3; i<3+cmd_cnt; i++)
		close(i);
	//command에 할당되어 있는 메모리 해제
	free(command);
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

		//입력된 line을 파이프를 기준으로 토큰화
		tokens = tokenize(line, 1);
		//명령어 실행 함수로 넘김
		executeCommand(tokens, cmd_cnt);
		//사용 완료된 tokens 메모리 해제
		freeToken(tokens);
	}
	return 0;
}
