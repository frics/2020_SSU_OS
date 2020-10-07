#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#include "processInfo.h"


int main(int argc, char *argv[])
{
	int check =0;
	struct processInfo procInfo;
	printf(1,"PID\tPPID\tSIZE\tNumber of Context Switch\n");
	for(int i=1; i<= get_max_pid(); i++){
		check = get_proc_info(i, &procInfo);
		if(check == -1)
			continue;
		printf(1,"%d\t%d\t%d\t%d\n", i, procInfo.ppid, procInfo.psize, procInfo.numberContextSwitches);
	}
	exit();
}
