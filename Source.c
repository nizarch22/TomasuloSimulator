#include "stdio.h"
#include "stdlib.h"
#include "InputOutput.h" // to be removed
#include "Tomasulo.h"


extern unsigned int cycles;
extern int bHalt;

// testing instructions from input + testing tomasulo config loading.
int main()
{
	createInstructions("memin.txt");
	Instruction in0, in1; in0 = instructions[0]; in1 = instructions[1];
	printf("%X,%X,%X,%X\n", in0.op, in0.dest, in0.src0, in0.src1);
	printf("%X,%X,%X,%X\n", in1.op, in1.dest, in1.src0, in1.src1);

	InitTomasulo("cfg.txt", "memin.txt");
	printf("add: %d, %d, %d\n", config.addUnits, config.addReserves, config.addDelay);
	printf("mul: %d, %d, %d\n", config.mulUnits, config.mulReserves, config.mulDelay);
	printf("div: %d, %d, %d\n", config.divUnits, config.divReserves, config.divDelay);

	int count = 0;
	while (instructions[count].op != END_OF_INSTRUCTION)
	{
		printf("%X,%X,%X,%X\n", instructions[count].op, instructions[count].dest, instructions[count].src0, instructions[count].src1);
		count++;
	}
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