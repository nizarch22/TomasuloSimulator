#include "stdio.h"
#include "stdlib.h"
#include "Tomasulo.h"


extern unsigned int cycles;
extern int bHalt;

// testing instructions from input + testing tomasulo config loading.
int main()
{
	InitTomasulo("cfg.txt", "memin.txt");
	while (bHalt == 0)
	{
		Fetch();
		Issue();
		Execute();
		Write();
		cycles++;
	}

	DestroyTomasulo();

	LogTomasulo("traceinst.txt", "tracecdb.txt");
}