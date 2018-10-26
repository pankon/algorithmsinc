/*
 * test- var_mem_man
 */

#include <stdio.h>
#include <string.h>

#include "var_mem_man.h"

#define MAX_STRING_SIZE 200
#define FILE_SIZE 0x300

struct sample_struct
{
	char data[20];
};

struct sample_struct_2
{
	char data[10];
};

int main(int argc, char **argv)
{
	VarMemMan_t *mem_man = NULL;
	void *data = NULL;
	struct sample_struct *data_0 = NULL;
	struct sample_struct_2 *data_1 = NULL;

	if (argc < 2)
	{
		printf("Usage: var_mem_man filename");
		return 1;
	}

	mem_man = VarMemManCreateOrLoadDb(argv[1], 0x21726176, FILE_SIZE);

	/*data_0 = VarMemManAlloc(mem_man, sizeof(struct sample_struct));
	if (NULL == data_0)
	{
		printf("could not alloc sample_struct\n");
	}
	else
	{
		strncpy(data_0->data, "this is a long string", 20); 
		VarMemManCommit(mem_man, data_0);
	}
	VarMemManFree(mem_man, data_0);
	*/

	{
		int i = 0;
		int n = strlen(argv[1]);

		for (i = 0; 3 > i; ++i)
		{
			printf("i: %d\n", i);
			data = VarMemManAlloc(mem_man, n + i);
			if (NULL != data)
			{
				memset(data, 0, n + i);
				strcpy(data, argv[1]);
				printf("loading data into object: %s\n", data);
				VarMemManCommit(mem_man, data);
				VarMemManCommit(mem_man, data);
			}
			if (i % 2 == 0)
			{
				VarMemManFree(mem_man, data);
			}
			else
			{
				VarMemManDelete(mem_man, data);
			}
			
			data = NULL;
		}
	}

	
	
	VarMemManDestroy(mem_man);
	mem_man = NULL;
	
	return 0;
}

