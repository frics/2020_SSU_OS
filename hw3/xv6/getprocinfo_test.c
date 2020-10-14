#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#include "processInfo.h"


int main(int argc, char *argv[])
{
	//processInfo가 올바르게 리턴 되었는지 확인
	int check =0;
	//ProcessInfo 구조체 선언
	struct processInfo procInfo;

	printf(1,"PID     PPID     SIZE     Number of Context Switch\n");
	//pid 1부터 현재 최대 pid까지 process 정보를 processInfo에 받아온다.
	for(int i=1; i<= get_max_pid(); i++){
		check = get_proc_info(i, &procInfo);
		//리턴값이 -1일 경우 다음으로 넘어간다.
		if(check == -1)
			continue;
		printf(1,"%d       %d        %d    %d\n", i, procInfo.ppid, procInfo.psize, procInfo.numberContextSwitches);
	}
	exit();
}
