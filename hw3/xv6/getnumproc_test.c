#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
	//출력문을 여기다 박는건가?
	printf(0, "Total Number of Active Processes : %d\n",get_num_proc());
	exit();
}
