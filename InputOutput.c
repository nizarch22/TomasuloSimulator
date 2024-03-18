#include "InputOutput.h"

void readFile(const char* dataPath, char fileStr[LEN_FILE])
{
	FILE* fp;
	char c;
	int count = 0;

	fp = fopen(dataPath, "r");
	while ((c = fgetc(fp)) != EOF)
	{
		fileStr[count] = c;
		count++;
	}
	fclose(fp);

	fileStr[count] = '\0';
	
}

void hexToNum(char* input)
{
	if (*input >= 48 && *input <= 57)
		*input -= 48;
	if (*input >= 65 && *input <= 70)
		*input -= 55;
	if (*input >= 97 && *input <= 102)
		*input -= 87;
}

void createInstructions(const char* meminPath)
{
	char fileStr[LEN_FILE];

	readFile(meminPath, fileStr);

	unsigned int countInstruction = 0;
	unsigned int count = 0;
	while (fileStr[count] != '\0')
	{
		if (fileStr[count] == '\n')
		{
			count++;
			continue;
		}

		// load one instruction
		count++; // skipping 0 padding
		memcpy(&instructions[countInstruction], &fileStr[count], 4*sizeof(char));
		hexToNum(&instructions[countInstruction].op);
		hexToNum(&instructions[countInstruction].dest);
		hexToNum(&instructions[countInstruction].src0);
		hexToNum(&instructions[countInstruction].src1);
		count += 7; // skipping the instruction and 0 padding

		countInstruction++;
	}
}

int getDictionaryIndex(char* value, char* dict[512], int size)
{
	if (value == NULL)
		return -1;
	for (int i = 0; i < size; i++)
	{
		if (!strcmp(value, dict[i]))
		{
			return i;
		}
	}
	return -1;
}

void initConfig(const char* cfgPath)
{
	char fileStr[LEN_CFG];
	char* configDict[] = { "add_nr_units", "mul_nr_units", "div_nr_units", "add_nr_reservation", "mul_nr_reservation", "div_nr_reservation", "add_delay", "mul_delay", "div_delay" };
	unsigned int configuration[9] = { -1 };

	readFile(cfgPath, fileStr);
	if (fileStr == NULL)
		return;

	char* varStr, * endVarStr;
	char* numPtr, * newLinePtr, * endPtr;
	newLinePtr = fileStr;
	int count = 0;
	int index = 0;
	varStr = NULL;
	do {
		numPtr = strchr(newLinePtr, '=');
		if (numPtr == NULL)
			break;
		// pointing to the start of the parameter string
		varStr = &newLinePtr[0];
		// making varStr readable as a parameter string.
		endVarStr = numPtr-1;
		while (*endVarStr == ' ')
			endVarStr--;
		endVarStr++;
		*endVarStr = '\0';

		// advancing towards the number
		numPtr++;

		// we point newLinePtr point to the end of line
		newLinePtr = strchr(numPtr, '\n');
		if (newLinePtr == NULL)
			break;
		newLinePtr++;
		// skip over empty lines
		while (*newLinePtr == '\n')
			newLinePtr++;
		// making numPtr readable as a number (value).
		endPtr = newLinePtr - 1;
		*endPtr = '\0';
		
		// finding index of parameter to write its value to config
		index = getDictionaryIndex(varStr, configDict, 9);
		if (index == -1)
			return;
		sscanf(numPtr, "%d", &configuration[index]);
		count++;
	} while (*newLinePtr != '\0');

	memcpy(&config, configuration, sizeof(Configuration));
}

void writeTraceInstr(const char* traceInstrPath, TraceInstr* traceLogInstr)
{
	FILE* fp;
	TraceInstr tl;
	Instruction instr;
	char *prefix[4] = {"ADD","ADD", "MUL", "DIV"};
	char opcode;
	unsigned int num;


	fp = fopen(traceInstrPath, "w");
	for (int i = 0; i < LEN_INSTRUCTIONS; i++)
	{
		tl = traceLogInstr[i];
		instr = tl.instr;
		if (tl.tag == 0)
		{
			break;
		}
		opcode = tl.tag & 7;
		num = tl.tag >> 3;
		
		if (i != 0)
			fprintf(fp, "\n");

		fprintf(fp, "0%X%X%X%X000\t%d\t%s%d\t%d\t%d\t%d\t%d", instr.op, instr.dest, instr.src0, instr .src1, tl.pc, prefix[(opcode - 2)], num, tl.cycleIssue, tl.cycleExStart, tl.cycleExEnd, tl.cycleWriteCDB);
	}
	fclose(fp);
}

void writeTraceCDB(const char* traceCDBPath, TraceCDB* traceLogCDB)
{
	FILE* fp;
	TraceCDB tl;
	char* prefix[4] = { "ADD","ADD", "MUL", "DIV" };
	char opcode;
	unsigned int num;


	fp = fopen(traceCDBPath, "w");
	for (int i = 0; i < LEN_INSTRUCTIONS; i++)
	{
		tl = traceLogCDB[i];
		if (tl.tag == 0)
		{
			break;
		}
		opcode = tl.tag & 7;
		num = tl.tag >> 3;

		if (i != 0)
			fprintf(fp, "\n");

		fprintf(fp, "%d\t%d\t%s\t%d\t%s%d", tl.cycleWriteCDB, tl.pc, prefix[(opcode - 2)], tl.data, prefix[(opcode - 2)], num);
	}
	fclose(fp);
}