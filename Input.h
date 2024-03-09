#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

#define LEN_INSTRUCTIONS 4097 // including 1 for the END_OF_INSTRUCTION flag
#define LEN_FILE 4096*5+1
#define LEN_CFG 23*9+1
#define LEN_PATH 14
#define END_OF_INSTRUCTION 999

// structures
typedef struct Instruction
{
	char op,dest, src0, src1;
	unsigned int cycleIssued, cycleExecStart, cycleExecEnd, cycleWrite;
}Instruction;

typedef struct Configuration
{
	unsigned int addUnits, mulUnits, divUnits;
	unsigned int addReserves, mulReserves, divReserves;
	unsigned int addDelay, mulDelay, divDelay;
}Configuration;

// global variables
Instruction instructions[LEN_INSTRUCTIONS];
Configuration config;

// functions
void readFile(char* dataPath, char fileStr[LEN_FILE]);
void createInstructions(char* meminPath);
void initConfig(const char* cfgPath);
