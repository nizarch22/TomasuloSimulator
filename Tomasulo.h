#pragma once
#include "InputOutput.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

// definitions
#define LEN_QUEUE 16
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define HALT 6
#define LEN_REGISTERS 16
//structs
typedef struct Station {
	unsigned int executeCount;
	int busy;
	int opcode;
	float Vj, Vk;
	int Qj, Qk;
	unsigned int cycleIssue; // Issue-Execute handling variable
	unsigned int traceIndexInstr; // logging variable
}Station;
typedef struct Registers
{
	unsigned int tag[LEN_REGISTERS];
	float F[LEN_REGISTERS];
}Registers;
typedef struct CDB
{
	float data;
	unsigned int tag;
	unsigned int busy;
}CDB;
typedef struct Table
{
	Station* stations;
	unsigned int len;
	unsigned int delay;
	unsigned int freeStationCount;
	unsigned int freeUnitCount;
	CDB cdb;
}Table;
typedef struct Queue
{
	unsigned int pc; // logging variable
	unsigned int cycleFetch; // Fetch-Issue handling variable
	Instruction instr;
	struct Queue* next;
}Queue;
typedef struct QueueStation
{
	Station* station;
	struct QueueStation* front;
	struct QueueStation* next;
}QueueStation;
// global variables
unsigned int cycles;
Table addTable;
Table mulTable;
Table divTable;
Registers regs;
int bHalt;
// Fetching
Queue* tail;
Queue* head;
unsigned int instrQSize;
unsigned int currentInstr;
//Execution
QueueStation* headStation;
QueueStation* tailStation;
unsigned int stationQSize;

//functions
void InitTomasulo(const char* cfgPath, const char* meminPath);
void DestroyTomasulo();
void Fetch();
void Issue();
void Execute();
void Write();

// Output handling
//structs
void LogTomasulo(const char* traceInstrPath, const char* traceCDBPath);
//global variables
TraceInstr traceLogInstr[LEN_INSTRUCTIONS];
TraceCDB traceLogCDB[LEN_INSTRUCTIONS];
unsigned int traceIndexInstr;
unsigned int traceIndexCDB;