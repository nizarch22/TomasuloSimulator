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
void initQueueStation()
{
	headStation = NULL;
	tailStation = NULL;
	stationQSize = 0;
}

void enqueueStation(Station* st)
{
	QueueStation* newcomer = (QueueStation*)malloc(sizeof(QueueStation));
	newcomer->station = st;
	newcomer->next = NULL;
	

	if (tailStation != NULL)
	{
		tailStation->next = newcomer;
		newcomer->front = tailStation;
	}
	tailStation = newcomer;

	if (stationQSize == 0)
	{
		newcomer->front = NULL;
		headStation = newcomer;
	}
	stationQSize++;
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

Station* dequeueStation()
{
	QueueStation* temp = headStation;
	if (headStation->next != NULL)
	{
		headStation = headStation->next;
	}
	else
	{
		headStation = NULL;
	}

	Station* val = temp->station;
	free(temp);

	stationQSize--;
	return val;
}

Instruction dequeue()
{
	Queue* temp = head;
	if (head->next != NULL)
	{
		head = head->next;
	}
	else
	{
		head = NULL;
	}

	Instruction val = temp->instr;
	free(temp);

	instrQSize--;
	return val;
}

QueueStation* popStation(QueueStation* current)
{
	if (current == NULL)
		return;
	QueueStation* front = current->front;
	QueueStation* back = current->next;
	if(front!=NULL)
		front->next = back;
	if(back!=NULL)
		back->front = front;
	QueueStation* temp = current;
	if (stationQSize == 1)
	{
		headStation = NULL;
		current = NULL;
	}
	else
	{
		current = back;
	}
	stationQSize--;
	free(temp);
	return current;
}

void InitTomasulo(const char* cfgPath, const char* meminPath)
{
	// intialize config parameters
	initConfig(cfgPath);
	// read, parse and load instructions
	createInstructions(meminPath);
	
	// initialize tables and their stations
	addTable.len = config.addReserves; 
	mulTable.len = config.mulReserves; 
	divTable.len = config.divReserves;
	addTable.freeStationCount = config.addReserves;
	mulTable.freeStationCount = config.mulReserves;
	divTable.freeStationCount = config.divReserves;

	addTable.stations = (Station*)calloc(config.addReserves, sizeof(Station));
	mulTable.stations = (Station*)calloc(config.mulReserves, sizeof(Station));
	divTable.stations = (Station*)calloc(config.divReserves, sizeof(Station));

	// intitialize registers
	for (int i = 0; i < 16; i++)
	{
		regs.F[i] = (float)i;
		regs.tag[i] = 0; // empty tag
	}

	//initialize Instruction Queue 
	initQueue();

	//Initialize Station Queue
	initQueueStation();

	//initialize current instr counter
	currentInstr = 0;

	//initialize cycles counter
	cycles = 0;

	//initialize the 3 CDBs
	CDBAdd.busy = 0;
	CDBMul.busy = 0;
	CDBDiv.busy = 0;
}

void DestroyTomasulo()
{
	//freeing stations of the tables
 	free(addTable.stations);
	free(mulTable.stations);
	free(divTable.stations);

	// freeing instruction queue
	Queue* temp;
	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
	// free station queue
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

		if (temp.freeStationCount == 0)
			break;
		for (int i = 0; i < temp.len;i++)
		{
			if (temp.stations[i].busy != 0)
				continue;

			instr = dequeue();
			temp.freeStationCount--;

			// load all busy,op, executeCount - all station values. 
			tempStation.busy = 1;
			tempStation.opcode = stationType;
			tempStation.executeCount = 0;

			//retrieve Qjk,Vjk values.
			tempStation.Qj = regs.tag[instr.src0];
			tempStation.Qk = regs.tag[instr.src1];

			if (tempStation.Qj == 0)
				tempStation.Vj = regs.F[instr.src0];
			else
				enqueueStation(&temp.stations[i]);

			if (tempStation.Qk == 0)
				tempStation.Vk = regs.F[instr.src1];
			else
				enqueueStation(&temp.stations[i]);
			// update stations array - done with tempStation
			temp.stations[i] = tempStation;

			//update register tag
			tag = i;
			tag <<= 3;
			tag += ((unsigned int)stationType);
			regs.tag[instr.dest] = tag;

			break;
		}
	}
}


void Execute()
{
	Station* st;
	for (int i = 0; i < config.addReserves;i++)
	{
		st = &addTable.stations[i];
		if (st->busy == 1 && st->Qj == 0 && st->Qk == 0)
			st->executeCount++;
	}
	for (int i = 0; i < config.mulReserves; i++)
	{
		st = &mulTable.stations[i];
		if (st->busy == 1 && st->Qj == 0 && st->Qk == 0)
			st->executeCount++;
	}
	for (int i = 0; i < config.divReserves; i++)
	{
		st = &divTable.stations[i];
		if (st->busy == 1 && st->Qj == 0 && st->Qk == 0)
			st->executeCount++;
	}
}

void Write()
{
	Station* st;
	for (int i = 0; i < config.addReserves; i++)
	{
		st = &addTable.stations[i];
		if (st->executeCount >= config.addDelay)
		{
			if (CDBAdd.busy == 0)
			{
				// Using this bus - busy
				CDBAdd.busy = 1;
				// Filling the data 
				CDBAdd.data = st->Vj + st->Vk;

				// Loading the tag
				CDBAdd.tag = i;
				CDBAdd.tag <<= 3;
				CDBAdd.tag += st->opcode;
			}
			break;
		}
	}

	//CDBAdd - stations
	QueueStation* temp = headStation;
	while (temp != NULL)
	{
		st = temp->station;
		// check both src register tags
		if (st->Qj == CDBAdd.tag)
		{
			st->Vj = CDBAdd.data;
			temp = popStation(temp);
		}
		if (st->Qk == CDBAdd.tag)
		{
			st->Vk = CDBAdd.data;
			temp = popStation(temp);
		}
		if (temp == NULL)
			break;
		temp = temp->next;
	}
	//CDBAdd - registers
	for (int i = 0; i < 16; i++)
	{
		if (regs.tag[i] == CDBAdd.tag && CDBAdd.tag!=0)
		{
			// update register
			regs.F[i] = CDBAdd.data;
			regs.tag[i] = 0;

			// Clear station
			st = &addTable.stations[(CDBAdd.tag >> 3)];
			st->busy = 0;
			st->executeCount = 0;
			st->opcode = 0;
			st->Qj = 0;
			st->Qk = 0;
			st->Vj = 0;
			st->Vk = 0;

			// CDB is done
			CDBAdd.busy = 0;
			CDBAdd.tag = 0;
		}
	}
	// do the same for the rest of Mul and Div
	for (int i = 0; i < config.mulReserves; i++)
	{
		st = &mulTable.stations[i];
		if (st->executeCount >= config.mulDelay)
		{
			if (CDBMul.busy == 0)
			{
				// Using this bus - busy
				CDBMul.busy = 1;
				// Filling the data 
				CDBMul.data = st->Vj * st->Vk;

				// Loading the tag
				CDBMul.tag = i;
				CDBMul.tag <<= 3;
				CDBMul.tag += st->opcode;
			}
		}
	}

	for (int i = 0; i < config.divReserves; i++)
	{
		st = &addTable.stations[i];
		if (st->executeCount >= config.divDelay)
		{
			if (CDBDiv.busy == 0)
			{
				// Using this bus - busy
				CDBDiv.busy = 1;
				// Filling the data 
				CDBDiv.data = st->Vj / st->Vk;

				// Loading the tag
				CDBDiv.tag = i;
				CDBDiv.tag <<= 3;
				CDBDiv.tag += st->opcode;
			}
		}
	}
}

void foo()
{
	int count = 100;
	while (count != 0)
	{
		Fetch();
		Issue();
		Execute();
		Write();
		cycles++;
		count--;
	}
	for (int i = 0; i < 16; i++)
	{
		printf("F%d: %f\n", i, regs.F[i]);
	}
}