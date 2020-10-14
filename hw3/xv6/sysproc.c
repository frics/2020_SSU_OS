#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "processInfo.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int
sys_hello(void)
{

	cprintf("helloxv6\n");
	return 0;
}
int
sys_hello_name(void)
{
	char *name;
	
	if(argstr(0, &name) <0)
		return -1;
	cprintf("hello %s\n", name);

	return 0;
}

int
sys_get_num_proc(void)
{
	return get_num_proc();
}
int
sys_get_max_pid(void)
{
	return get_max_pid();
}
int
sys_get_proc_info(void)
{
	int pid;
	struct processInfo *procInfo;
	if(argint(0, &pid) < 0 || argptr(1, (void*)&procInfo, sizeof(*procInfo)))
		return -1;

	return get_proc_info(pid, procInfo);
}

//혹시 스케줄링중에 버그 발생하면
//set_prio를 proc.c로 넣어서 테스트
int 
sys_set_prio(void)
{
	int n;
	if(argint(0, &n) <0)
		return -1;
	return set_prio(n);
	//myproc()-> priority = n;
	//return 0; 
}

int 
sys_get_prio(void)
{
	//return myproc()->priority;
	return get_prio();
}

