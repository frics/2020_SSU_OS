#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include<wait.h>

int setpgid(pid_t pid, pid_t pgid);
struct dirent *readdir(DIR *dirp);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int errno;

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64



int checkpipe(char *line)
{ int i,count=0;
	for(i=0;i<strlen(line);i++)
	{if(line[i] == '|')
		count++;
	}
	return count;
}


char **pipeparse(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for(i =0; i < strlen(line); i++){

		char readChar = line[i];
		//printf("readChar : %c\n", readChar);
	//	if(line[i-1] == '|'){
	//		printf("찾았다 파이프\n");    
	//		continue;

	//	}
		if (readChar == '|' || readChar == '\n' || readChar == '\t'){
			printf("구분 문자임 : %c\n", readChar);
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				printf("토큰 박아넣어 %s\n", token);
				tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				for(int j=0; tokens[j]!=NULL; j++)
					printf("넣은거 확인: %s\n", tokens[j]);

				tokenIndex = 0;
			}
		} else {
			printf("구분문자 아님 : %c\n", readChar);
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL ;
	printf("파이프 토큰 뽑아내기\n ");
	for(int i=0; tokens[i] != NULL; i++){
		printf("pipe token : (%s)", tokens[i]);
	}
	printf("\n");
	return tokens;

}

char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for(i =0; i < strlen(line); i++){

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		} else {
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL ;
	return tokens;
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];
	char  **tokens;
	int i;

	printf("\n\n------ Welcome to the shell -----\n\n");

	int back=0;
	while(1) {

		bzero(line, sizeof(line));
		printf("<<ishanarya0>> ");

		scanf("%[^\n]", line);
		getchar();

		if(strcmp(line,"\0")==0)
			continue;


		line[strlen(line)] = '\n';

		if(checkpipe(line)!=0)
		{ int i,num_pipe = checkpipe(line),status;

			tokens = pipeparse(line);
			char **str;
			int fds[2*num_pipe];
			for(i=0;i<(2*num_pipe);i+=2){
				printf("pipe opne\n");
				if(pipe(fds+i)== -1)
					printf("pipe fail\n");
			}
			int j;
			//for(j=0;tokens[j]!=NULL;j++)
			//	printf("%s\n",tokens[j]);
			int g=0;

			int cnt = 0;
			for(i=0;i<num_pipe+1;i++){
				//파이프 생성
				//그냥 하나씩 하는것도 시도해 보자
	//			pipe(fds+i);

				if(i==num_pipe)
				{ int len = strlen(tokens[i]);
					tokens[i][len] = '\n';
					printf("^^^^^^%s^^^^^^^\n", tokens[i]);
				}
				str = tokenize(tokens[i]);
				if(fork()==0)
				{ 
					if(i==0){
						dup2(fds[1],1);
						close(fds[1]);
	//					close(pipes[2*i]);
	//					close(pipes[(2*i)+1]);
					}
					else if(i==num_pipe){
						dup2(fds[(2*i)-2],0);
//						close(fds[(2*i)-2]);
					}
					else{
						dup2(fds[g],0);
						dup2(fds[g+3],1);
						g=g+2;
//						close(fds[g]);
//						close(fds[g+3]);
					}
					for(j=0;j<(2*num_pipe);j=j+2){
						if(close(fds[j]) == -1 || close(fds[j+1]) == -1)
								printf("close fail\n");
					}
			/*		for(j=0;j<(2*num_pipe);j++)
						close(pipes[j]);
*/
					if(execvp(str[0],str)<0)
						printf("INVALID COMMAND\n");
				}
			}
		for(j=0;j<(2*num_pipe);j+=2){
				if(close(fds[j]) == -1 || close(fds[j+1]))
					printf("pipe close error\n");
			}
			for (i = 0; i < num_pipe+1; i++){
				wait(&status);
			}
	//		printf("wait is over");

			continue;
		}

		tokens = tokenize(line);


		if(strcmp(tokens[0],"exit")==0)
		{printf("***********EXITING*************\n");
			exit(0);}

		if(strcmp(tokens[0],"cd")==0)
		{ char s[1000];
			if(chdir(tokens[1])==0)
				printf("changed dir. to : %s\n",getcwd(s,1000));
			continue;}

		if(strcmp(tokens[0],"pwd")==0)
		{ char s[1000];

			printf("current dir. : %s\n",getcwd(s,1000));
			continue;}
		if(strcmp(tokens[0],"ls")==0)
		{  {

			   DIR *dirp = opendir(".");
			   struct dirent *dp;

			   while ((dp = readdir(dirp)) != NULL)
				   printf("%s  ", dp->d_name);

			   closedir(dirp);
			   printf("\n");
			   continue;
		   }}

		pid_t pid = fork();
		for(i=0;tokens[i+1]!=NULL;i++);

		back =0;
		if(strcmp(tokens[i],"&")==0)
		{   
			back =1;
			tokens[i]=NULL;
		}

		if (pid == -1) {
			printf("\nFailed");
			return 0;
		} else if (pid == 0) {
			printf("parent :%d \n", getppid());
			printf("chlid :%d \n", getpid());
			if (execvp(tokens[0],tokens) < 0) {
				printf("\nCould not execute command..\n");
			}
			exit(0);
		} else {
			if(back==0)
				waitpid(pid,NULL,0);

		}



		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
