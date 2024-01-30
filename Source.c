#include "stdio.h"
#include "stdlib.h"

#include "Input.h"


// testing instructions from input + testing tomasulo config loading.
int main()
{
	createInstructions("memin.txt");

	int count = 0;
	while (instructions[count].op != END_OF_INSTRUCTION)
	{
		printf("%c,%c,%c,%c\n", instructions[count].dest, instructions[count].src0, instructions[count].src1, instructions[count].op);
		count++;
	}

	initTomasulo("cfg.txt");
	printf("add: %d, %d, %d\n", tomasulo.addUnits, tomasulo.addReserves, tomasulo.addDelay);
	printf("div: %d, %d, %d\n", tomasulo.divUnits, tomasulo.divReserves, tomasulo.divDelay);
	printf("mul: %d, %d, %d\n", tomasulo.mulUnits, tomasulo.mulReserves, tomasulo.mulDelay);
}
