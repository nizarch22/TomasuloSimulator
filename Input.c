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


void createInstructions(char* meminPath)
{
	char fileStr[LEN_FILE];

	readFile(meminPath, fileStr);

	unsigned int countInstruction = 0;
	unsigned int count = 0;
	while (fileStr[count] != '\0')
	{
		// load one instruction
		memcpy(&instructions[countInstruction], &fileStr[count], sizeof(Instruction));

		count += 4;
		if(fileStr[count]=='\n')
			count++;

		countInstruction++;
	}

}

void initTomasulo(char* cfgPath)
{
	char fileStr[LEN_CFG];
	unsigned int config[9] = { -1 };

	readFile(cfgPath, fileStr);

	char* numPtr, *newLinePtr, *endPtr;
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

		sscanf(numPtr, "%d", &config[count]);
		count++;
	} while (*newLinePtr != '\0');
	sscanf(numPtr, "%d", &config[count]);

	printf("Config: ");
	for (int i = 0; i < 9; i++)
		printf("%d,",config[i]);
	printf("\n");
	memcpy(&tomasulo, config, sizeof(Tomasulo));
}