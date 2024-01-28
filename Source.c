#include "stdio.h"
#include "stdlib.h"

#include "Input.h"


// testing instructions from input + testing tomasulo config loading.
int main()
{
	createInstructions("memin.txt");
	Instruction in0, in1; in0 = instructions[0]; in1 = instructions[1];
	printf("%c,%c,%c,%c\n", in0.dest, in0.src0, in0.src1, in0.op);
	printf("%c,%c,%c,%c\n", in1.dest, in1.src0, in1.src1, in1.op);

	initTomasulo("cfg.txt");
	printf("mul: %d, %d, %d\n", tomasulo.addUnits, tomasulo.addReserves, tomasulo.addDelay);
	printf("div: %d, %d, %d\n", tomasulo.divUnits, tomasulo.divReserves, tomasulo.divDelay);
	printf("mul: %d, %d, %d\n", tomasulo.mulUnits, tomasulo.mulReserves, tomasulo.mulDelay);
}