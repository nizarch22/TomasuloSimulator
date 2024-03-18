#include "stdio.h"
#include "stdlib.h"
#include "Tomasulo.h"


extern unsigned int cycles;
extern int bHalt;

// testing instructions from input + testing tomasulo config loading.
int main(int argc, char* argv[])
{
	//int argc = 6;
	//char* argv[] = { "sim.exe","cfg.txt","memin.txt","regout.txt","traceinst.txt","tracecdb.txt" };
	if (argc != 6)
	{
		printf("Invalid inputs!\n");
		return -1;
	}
	
	for (int i = 0; i < 6; i++)
	{
		printf(argv[i]);
		printf("\n");
	}

	int bInit;
	bInit = InitTomasulo(argv[1], argv[2]);
	if (bInit == -1)
	{
		printf("Error! Could not initialize Tomasulo.\n");
		return -1;
	}
	printf("init success");
	while (bHalt == 0)
	{
		Fetch();
		Issue();
		Execute();
		Write();
		cycles++;

	}

	DestroyTomasulo();

	LogTomasulo(argv[3],argv[4], argv[5]);

}