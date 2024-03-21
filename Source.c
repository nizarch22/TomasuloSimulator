#include "stdio.h"
#include "stdlib.h"
#include "Tomasulo.h"

extern unsigned int cycles;
extern int bHalt;


int main(int argc, char* argv[])
{
	// checking arguments are there.
	if (argc != 6)
	{
		printf("Invalid inputs! Need 6 inputs.\n");
		return -1;
	}

	// initializing all config and instructions. Also initializing instances specific to the Tomasulo program.
	int bInit; // error checking variable.
	bInit = InitTomasulo(argv[1], argv[2]);
	if (bInit == -1)
	{
		printf("Error! Could not initialize Tomasulo.\n");
		return -1;
	}

	// Keeps running until we encounter a HALT instruction.
	while (bHalt == 0)
	{
		Fetch();
		Issue();
		Execute();
		Write();
		cycles++;
	}

	// Release memory
	DestroyTomasulo();

	// log instructions and CDB events.
	LogTomasulo(argv[3],argv[4], argv[5]);

}