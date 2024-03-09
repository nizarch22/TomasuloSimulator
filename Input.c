#include "Input.h"

void readFile(char* dataPath, char fileStr[LEN_FILE])
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
	sscanf(numPtr, "%d", &configuration[count]);

	memcpy(&config, configuration, sizeof(Configuration));
}