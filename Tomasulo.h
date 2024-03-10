#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

// definitions
#define QUEUE_LEN 16 
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define HALT 6
//structs
typedef struct Station {
	int executeCount;
	int busy;
	int opcode;
	float Vj, Vk;
	int Qj, Qk;
}Station;
typedef struct Registers
{
	unsigned int tag[16];
	float F[16];
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
	unsigned int freeStationCount;
}Table;
typedef struct Queue
{
	unsigned int cycleFetch;
	Instruction instr;
	struct Queue* next;
}Queue;
typedef struct QueueStation
{
	unsigned int cycleFetch;
	Station* station;
	struct Queue* front;
	struct Queue* next;
}QueueStation;
// global variables
int cycle;
Table addTable;
Table mulTable;
Table divTable;
Registers regs;
CDB CDBAdd;
CDB CDBMul;
CDB CDBDiv;
// Fetching
Queue* tail;
Queue* head;
unsigned int instrQSize;
unsigned int currentInstr;
unsigned int cycles;
//Execution
QueueStation* headStation;
QueueStation* tailStation;
unsigned int stationQSize;

//functions
void InitTomasulo(const char* cfgPath, const char* meminPath);
void DestroyTomasulo();
//debug
void foo();