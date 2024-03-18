#include "stdio.h"
#include "stdlib.h"
#include "Tomasulo.h"


extern unsigned int cycles;
extern int bHalt;

// testing instructions from input + testing tomasulo config loading.
int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		printf("Invalid inputs!\n");
	}
	
	InitTomasulo(argv[1], argv[2]);
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