#include "InputOutput.h"
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
	if (instrQSize >= LEN_QUEUE)
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
QueueStation* popStation(QueueStation* current)
{
	if (current == NULL)
		return NULL;
	// connect front and back
	QueueStation* front = current->front;
	QueueStation* back = current->next;
	if(front!=NULL)
		front->next = back;
	if(back!=NULL)
		back->front = front;

	// make sure not to delete the head
	if (current == headStation)
	{
		headStation = NULL;
	}
	else
	{
		free(current);
	}
	stationQSize--;
	return back;

}

void executeTable(Table* table)
{
	Station* st;
	for (unsigned int i = 0; i < table->len; i++)
	{
		st = &(table->stations[i]);
		if (cycles == st->cycleIssue)
			continue;
		if (st->busy == 1 && st->Qj == 0 && st->Qk == 0)
		{
			//Logging - instr
			if (st->executeCount == 0)
			{
				traceLogInstr[st->traceIndexInstr].cycleExStart = cycles;
				traceLogInstr[st->traceIndexInstr].cycleExEnd = cycles + table->delay - 1;
			}

			st->executeCount++;
		}
	}
}
void writeTable(Table* table)
{
	// Write to CBD
	Station* st;
	for (unsigned int i = 0; i < table->len; i++)
	{
		st = &(table->stations)[i];
		if (st->executeCount > table->delay)
		{
			if (table->cdb.busy == 0)
			{
				// Using this bus - busy
				table->cdb.busy = 1;
				// Filling the data

				switch (st->opcode)
				{
				case 2: // ADD
					table->cdb.data = st->Vj + st->Vk;
					break;
				case 3: // SUB
					table->cdb.data = st->Vj - st->Vk;
					break;
				case 4: // MUL
					table->cdb.data = st->Vj * st->Vk;
					break;
				case 5: // DIV
					table->cdb.data = st->Vj / st->Vk;
					break;
				default:
					break;
				}
				// Loading the tag
				table->cdb.tag = i + 1;
				table->cdb.tag <<= 3;
				table->cdb.tag += st->opcode;

				// Logging - CDB
				traceLogCDB[traceIndexCDB].cycleWriteCDB = cycles;
				traceLogCDB[traceIndexCDB].data = (int)table->cdb.data;
				traceLogCDB[traceIndexCDB].pc = traceLogInstr[st->traceIndexInstr].pc;
				traceLogCDB[traceIndexCDB].tag = table->cdb.tag;
				traceIndexCDB++;

				// Logging - Instr
				traceLogInstr[st->traceIndexInstr].cycleWriteCDB = cycles;
			}
			break;
		}
	}

	//Add - write Vjk to issued stations waiting for Vjk
	QueueStation* temp = headStation;
	while (temp != NULL)
	{
		if (table->cdb.tag == 0)
			break;
		st = temp->station;
		// check both src register tags
		if (st->Qj == table->cdb.tag)
		{
			st->Vj = table->cdb.data;
			temp = popStation(temp);
			st->Qj = 0;
		}
		if (st->Qk == table->cdb.tag)
		{
			st->Vk = table->cdb.data;
			temp = popStation(temp);
			st->Qk = 0;
		}
		if (temp == NULL)
			break;
		temp = temp->next;
	}
	//Add - write to register evaluator. And empty the stations (+the CDB) - as we are done with them.
	for (int i = 0; i < LEN_REGISTERS; i++)
	{
		if (regs.tag[i] == table->cdb.tag && table->cdb.tag != 0)
		{
			// update register
			regs.F[i] = table->cdb.data;
			regs.tag[i] = 0;

			break;
		}
	}

	// Clear CDB, station after writing is finished (to register and to stations that needed the value).
	// Also make it known that this station is now free.
	if (table->cdb.tag != 0)
	{
		// Clear station
		st = &table->stations[(table->cdb.tag >> 3) - 1];
		st->busy = 0;
		st->executeCount = 0;
		st->cycleIssue = 0;
		st->opcode = 0;
		st->Qj = 0;
		st->Qk = 0;
		st->Vj = 0;
		st->Vk = 0;
		st->traceIndexInstr = 0;
		table->freeStationCount++;
		table->freeUnitCount++;

		// CDB is done
		table->cdb.data = 0;
		table->cdb.busy = 0;
		table->cdb.tag = 0;
	}
}

int InitTomasulo(const char* cfgPath, const char* meminPath)
{
	// intialize config parameters
	int bRead = initConfig(cfgPath);
	if (bRead == -1)
		return -1;
	// read, parse and load instructions
	bRead=createInstructions(meminPath);
	if (bRead == -1)
		return -1;
	// initialize tables and their stations
	addTable.len = config.addReserves; 
	mulTable.len = config.mulReserves; 
	divTable.len = config.divReserves;
	addTable.freeStationCount = config.addReserves;
	mulTable.freeStationCount = config.mulReserves;
	divTable.freeStationCount = config.divReserves;
	addTable.freeUnitCount = config.addUnits;
	mulTable.freeUnitCount = config.mulUnits;
	divTable.freeUnitCount = config.divUnits;
	//initialize unit busy array memory


	// intitialize stations memory
	addTable.stations = (Station*)calloc(config.addReserves, sizeof(Station));
	mulTable.stations = (Station*)calloc(config.mulReserves, sizeof(Station));
	divTable.stations = (Station*)calloc(config.divReserves, sizeof(Station));
	// initialize the 3 CDBs
	addTable.cdb.busy = 0;
	mulTable.cdb.busy = 0;
	divTable.cdb.busy = 0;
	// initialize the delay values
	addTable.delay = config.addDelay;
	mulTable.delay = config.mulDelay;
	divTable.delay = config.divDelay;

	// intitialize registers
	for (int i = 0; i < LEN_REGISTERS; i++)
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

	//initialize Halt flag, bHalt
	bHalt = 0;

	//initialize traceLogInstr index
	traceIndexInstr = 0;
	//initialize traceLogCDB index
	traceIndexCDB = 0;
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
	// free station queue - note that it is supposed to be empty upon successful completion.
	QueueStation* temp1;
	while (headStation != NULL)
	{
		temp1 = headStation;
		headStation = headStation->next;
		free(temp1);
	}
}

void Fetch()
{
	// HALT exception handling
	if (head != NULL)
	{
		if (head->instr.op == HALT)
		{
			// check if all instructions are finished
			if (addTable.freeStationCount == addTable.len && mulTable.freeStationCount == mulTable.len && divTable.freeStationCount == divTable.len)
			{
				bHalt = 1;
			}
			return;
		}
	}
	// load 2 instructions onto instruction queue
	if (instrQSize < LEN_QUEUE && currentInstr < LEN_INSTRUCTIONS)
	{
		enqueue(instructions[currentInstr]);
		tail->cycleFetch = cycles;
		tail->pc = currentInstr;

		// HALT exception handling
		if (tail->instr.op == HALT)
			return;
		currentInstr++;
	}
	if (instrQSize < LEN_QUEUE && currentInstr < LEN_INSTRUCTIONS)
	{
		enqueue(instructions[currentInstr]);
		tail->cycleFetch = cycles;
		tail->pc = currentInstr;

		currentInstr++;
	}
}

void Issue()
{	
	int stationType;
	Table* temp = NULL;
	Station tempStation;
	Instruction instr;
	unsigned int tag;
	unsigned int pc;

	//search for available station
	for (int j = 0; j < 2; j++)
	{
		if (head == NULL)
			break;

		// we do not issue at the cycle same as Fetch
		if (cycles == head->cycleFetch)
			return;
		// we do not wish to issue anymore after HALT
		if (head->instr.op == HALT)
			return;
		stationType = head->instr.op;
		switch (stationType)
		{
		case 2:
			temp = &addTable;
			break;
		case 3:
			temp = &addTable;
			break;
		case 4:
			temp = &mulTable;
			break;
		case 5:
			temp = &divTable;
			break;
		default:
			break;
		}
		if (temp == NULL)
			break;

		if (temp->freeStationCount == 0 || temp->freeUnitCount==0)
			break;
		for (unsigned int i = 0; i < temp->len;i++)
		{
			if (temp->stations[i].busy != 0)
				continue;

			// dequeue instruction from instructionQueue and place into a station.
			pc = head->pc; // Logging - instr
			instr = dequeue();

			// load all busy,op, executeCount, cycleIssue, traceIndexInstr(Logging) - all station values. 
			tempStation.busy = 1;
			tempStation.opcode = stationType;
			tempStation.executeCount = 0;
			tempStation.cycleIssue = cycles;
			tempStation.traceIndexInstr = traceIndexInstr; // Logging - instr
			//retrieve Qjk,Vjk values.
			tempStation.Qj = regs.tag[instr.src0];
			tempStation.Qk = regs.tag[instr.src1];

			if (tempStation.Qj == 0)
				tempStation.Vj = regs.F[instr.src0];
			else
				enqueueStation(&(temp->stations[i]));

			if (tempStation.Qk == 0)
				tempStation.Vk = regs.F[instr.src1];
			else
				enqueueStation(&(temp->stations[i]));
			// update stations array - done with tempStation
			temp->stations[i] = tempStation;

			//update register tag
			tag = i+1; 
			tag <<= 3;
			tag += ((unsigned int)stationType);
			regs.tag[instr.dest] = tag;

			// declaring this station as busy (not free) makes our freeStationCount drop.
			temp->freeStationCount--;
			temp->freeUnitCount--;
			// Logging - instr
			traceLogInstr[traceIndexInstr].cycleIssue = cycles;
			traceLogInstr[traceIndexInstr].instr = instr;
			traceLogInstr[traceIndexInstr].pc = pc;
			traceLogInstr[traceIndexInstr].tag = tag;

			traceIndexInstr++;

			break;
		}
	}
}

void Execute()
{
	// Execute Add
	executeTable(&addTable);
	// Execute Mul
	executeTable(&mulTable);
	// Execute Div
	executeTable(&divTable);
}

void Write()
{
	// Add table
	writeTable(&addTable);
	// Mul Table
	writeTable(&mulTable);
	// Div Table
	writeTable(&divTable);
}

void LogTomasulo(const char* regoutPath,const char* traceInstrPath, const char* traceCDBPath)
{
	writeTraceRegout(regoutPath, regs.F);
	writeTraceInstr(traceInstrPath, traceLogInstr);
	writeTraceCDB(traceCDBPath, traceLogCDB);
}
