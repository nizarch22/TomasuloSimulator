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
	instructions[countInstruction].op = END_OF_INSTRUCTION;
}


void initConfig(const char* cfgPath)
{
	char fileStr[LEN_CFG];
	unsigned int configuration[9] = { -1 };

	readFile(cfgPath, fileStr);

	char* numPtr, * newLinePtr, * endPtr;
	newLinePtr = fileStr;
	int count = 0;
	do {
		numPtr = strchr(newLinePtr, '=');
		if (numPtr == NULL)
			break;
		numPtr++;

		newLinePtr = strchr(numPtr, '\n');
		if (newLinePtr == NULL)
			break;
		newLinePtr++;

		endPtr = newLinePtr - 1;
		*endPtr = '\0';

		sscanf(numPtr, "%d", &configuration[count]);
		count++;
	} while (*newLinePtr != '\0');
	if (numPtr != NULL)
	{
		sscanf(numPtr, "%d", &configuration[count]);
	}
	

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

		fprintf(fp, "0%X%X%X%X000 %d %s%d %d %d %d %d", instr.op, instr.dest, instr.src0, instr .src1, tl.pc, prefix[(opcode - 2)], num, tl.cycleIssue, tl.cycleExStart, tl.cycleExEnd, tl.cycleWriteCDB);
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
	for (int i = 0; i < 3*LEN_INSTRUCTIONS; i++)
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

		fprintf(fp, "%d %d %s %d %s%d", tl.cycleWriteCDB, tl.pc, prefix[(opcode - 2)], tl.data, prefix[(opcode - 2)], num);
	}
	fclose(fp);
}