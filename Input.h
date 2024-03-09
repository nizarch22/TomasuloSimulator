#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

#define LEN_INSTRUCTIONS 4096
#define LEN_FILE 4096*5+1
#define LEN_CFG 23*9+1
#define LEN_PATH 14

// structures
typedef struct Instruction
{
	char dest, src0, src1, op;
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
