#include "stdio.h"
#include "stdlib.h"
#include "Input.h" // to be removed
#include "Tomasulo.h"


// testing instructions from input + testing tomasulo config loading.
int main()
{
	createInstructions("memin.txt");
	Instruction in0, in1; in0 = instructions[0]; in1 = instructions[1];
	printf("%c,%c,%c,%c\n", in0.dest, in0.src0, in0.src1, in0.op);
	printf("%c,%c,%c,%c\n", in1.dest, in1.src0, in1.src1, in1.op);

	InitTomasulo("cfg.txt", "memin.txt");
	printf("add: %d, %d, %d\n", config.addUnits, config.addReserves, config.addDelay);
	printf("mul: %d, %d, %d\n", config.mulUnits, config.mulReserves, config.mulDelay);
	printf("div: %d, %d, %d\n", config.divUnits, config.divReserves, config.divDelay);

	int count = 0;
	while (instructions[count].op != END_OF_INSTRUCTION)
	{
		printf("%c,%c,%c,%c\n", instructions[count].op, instructions[count].dest, instructions[count].src0, instructions[count].src1);
		count++;
	}
	foo();
	//while ()
	//{
	//	FETCH();
	//


}