#include "Input.h"
#include "Tomasulo.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


void initQueue()
{
	head = NULL;
	tail = NULL;
	instrQSize = 0;
}
void enqueue(Instruction instr)
{
	if (instrQSize >= QUEUE_LEN)
		return;

	Queue* newcomer = (Queue*)malloc(sizeof(Queue));
	memcpy(&newcomer->instr, &instr, sizeof(Instruction));
	newcomer->next = NULL;

	if(tail!=NULL)
		tail->next = newcomer;
	tail = newcomer;

	if (instrQSize == 0)
		head = newcomer;
	instrQSize++;
}

Instruction dequeue()
{
	Queue* temp = head;
	if(head->next!=NULL)
		head = head->next;

	Instruction val = temp->instr;
	free(temp);

	instrQSize--;
	return val;
}



void InitTomasulo(const char* cfgPath, const char* meminPath)
{
	// intialize config parameters
	initConfig(cfgPath);
	// read, parse and load instructions
	createInstructions(meminPath);
	
	// initialize stations
	addTable.len = config.addReserves; 
	mulTable.len = config.mulReserves; 
	divTable.len = config.divReserves;
	addTable.freeStationCount = config.addReserves;
	mulTable.freeStationCount = config.mulReserves;
	divTable.freeStationCount = config.divReserves;

	addTable.stations = (Station*)alloca(config.addReserves * sizeof(Station));
	mulTable.stations = (Station*)alloca(config.mulReserves * sizeof(Station));
	divTable.stations = (Station*)alloca(config.divReserves * sizeof(Station));

	for (int i = 0; i < config.addReserves; i++)
	{
		addTable.stations[i].busy = 0;
		addTable.stations[i].executeCount = 0;
	}
	for (int i = 0; i < config.mulReserves; i++)
	{
		mulTable.stations[i].busy = 0;
		mulTable.stations[i].executeCount = 0;
	}
	for (int i = 0; i < config.divReserves; i++)
	{
		divTable.stations[i].busy = 0;
		divTable.stations[i].executeCount = 0;
	}

	// intitialize registers
	for (int i = 0; i < 16; i++)
	{
		regs.F[i] = (float)i;
		regs.tag[i] = 0; // empty tag
	}

	//initialize Instruction Queue 
	initQueue();

	//initialize current instr counter
	currentInstr = 0;

	//initialize cycles counter
	cycles = 0;
}

void foo()
{

}


void Fetch()
{
	// HALT exception handling
	if (head != NULL)
	{
		if (head->instr.op == HALT)
			return;
	}
	// load 2 instructions onto instruction queue
	if (instrQSize < QUEUE_LEN && currentInstr < LEN_INSTRUCTIONS)
	{
		enqueue(instructions[currentInstr]);
		tail->cycleFetch = cycles;

		// HALT exception handling
		if (tail->instr.op == HALT)
			return;

		currentInstr++;
	}
	if (instrQSize < QUEUE_LEN && currentInstr < LEN_INSTRUCTIONS)
	{
		enqueue(instructions[currentInstr]);
		tail->cycleFetch = cycles;

		currentInstr++;
	}
}

void Issue()
{	
	int flag = 3;
	int stationType;
	Table temp = { 0 };
	Station tempStation;
	Instruction instr;
	unsigned int tag;

	//search for available station
	for (int j = 0; j < 2; j++)
	{
		if (head == NULL)
			break;

		// we do not issue at the cycle same as Fetch
		if (cycles == head->cycleFetch)
			return;

		if (head->instr.op == HALT)
			break;
		stationType = head->instr.op;

		switch (stationType)
		{
		case 2:
			temp = addTable;
			break;
		case 3:
			temp = addTable;
			break;
		case 4:
			temp = mulTable;
			break;
		case 5:
			temp = divTable;
			break;
		default:
			break;
		}
		if (temp.len == 0)
			break;

		for (int i = 0; i < temp.len;i++)
		{
			if (temp.stations[i].busy != 0)
				continue;

			instr = dequeue();
			temp.freeStationCount--;

			// load all busy,op, station values. 
			tempStation.busy = 1;
			tempStation.opcode = stationType;

			//retrieve Qjk,Vjk values.
			tempStation.Qj = regs.tag[instr.src0];
			tempStation.Qk = regs.tag[instr.src1];

			if (tempStation.Qj == 0)
				tempStation.Vj = regs.F[instr.src0];
			if (tempStation.Qk == 0)
				tempStation.Vk = regs.F[instr.src1];
			// update stations array - done with tempStation
			temp.stations[i] = tempStation;

			//update register tag
			tag = i;
			tag <<= 3;
			tag += ((unsigned int)stationType);
			regs.tag[instr.dest] = tag;

			break;
		}
		// FLAG - no stations available
		j = flag;
	}


}