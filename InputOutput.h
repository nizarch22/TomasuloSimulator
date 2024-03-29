#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

#define LEN_INSTRUCTIONS 4096
#define LEN_FILE 4096*9+1
#define LEN_CFG 512
#define LEN_REGISTERS 16

// structures
typedef struct Instruction
{
	char op,dest, src0, src1;
}Instruction;

typedef struct Configuration
{
	unsigned int addUnits, mulUnits, divUnits;
	unsigned int addReserves, mulReserves, divReserves;
	unsigned int addDelay, mulDelay, divDelay;
}Configuration;
typedef struct TraceInstr
{
	Instruction instr;
	unsigned int cycleIssue;
	unsigned int cycleExStart;
	unsigned int cycleExEnd;
	unsigned int cycleWriteCDB;
	unsigned int pc;
	unsigned int tag; //tag(Instr),cdbname(CDB)
}TraceInstr;
typedef struct TraceCDB
{
	float data;
	unsigned int cycleWriteCDB;
	unsigned int pc;
	unsigned int tag; //tag(Instr),cdbname(CDB)
}TraceCDB;
// global variables
Instruction instructions[LEN_INSTRUCTIONS];
Configuration config;


// functions
int createInstructions(const char* meminPath); // loads HEX instructions and parses them into numeric form.
int initConfig(const char* cfgPath); // loads and matches configuration variables of the Tomasulo program.
void writeTraceRegout(const char* regoutPath, float* F); // log entire register values.
void writeTraceInstr(const char* traceInstrPath, TraceInstr* traceLogInstr); // log every instruction executed and finished.
void writeTraceCDB(const char* traceCDBPath, TraceCDB* traceLogCDB); // log every CDB write event.