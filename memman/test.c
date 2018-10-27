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

void TestCommitAndDelete(VarMemMan_t *mem_man, char *string, int do_delete)
{
	int n = 0;
	void *data = NULL;

	n = strlen(string);

	printf("string: %s, do_delete = %d\n", string, do_delete);
	data = VarMemManAlloc(mem_man, n);
	if (NULL != data)
	{
		memset(data, 0, n);
		strcpy(data, string);
		printf("loading data into object: %s\n", data);
		VarMemManCommit(mem_man, data);
	}
	else
	{
		printf("alloc failed here\n");
	}

	if (do_delete)
	{
		VarMemManDelete(mem_man, data);
	}
	else
	{
		VarMemManFree(mem_man, data); // VarMemManFree
	}
	
	data = NULL;
}

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

	TestCommitAndDelete(mem_man, "nana rules", 0);
	TestCommitAndDelete(mem_man, "deleted", 1);
	TestCommitAndDelete(mem_man, "okey", 0);	
	TestCommitAndDelete(mem_man, "dokey", 0);
	
	VarMemManDestroy(mem_man);
	mem_man = NULL;
	
	return 0;
}

