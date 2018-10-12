/*
 * ch3 - linked list char database
 * simple version - fixed size pointer, wasted space in padding
 */

#include <stdio.h>
#include <stdlib.h>

#define FILE_SIZE 0x300
#define NULL_TERM 0xffff
#define MAX_STRING_SIZE 0x100
#define DEBUG 0
#define DBG(COND, statement) if (COND) statement

struct head
{
	uint free;
	uint allocated;
};

struct data
{
	uint next; // up to MAX_INT / (sizeof(int) + 1) chars
	char ch;
};

int SafeSeek(FILE *fp, size_t offset)
{
	DBG(DEBUG, printf("trying to seek to: %x\n", offset));
	if (NULL_TERM <= offset)
	{
		return 0;
	}

	fseek(fp, offset, SEEK_SET);
	return 1;
}

struct data DataRead(FILE *fp, size_t offset)
{
	struct data readData = {NULL_TERM, 0};

	if (SafeSeek(fp, offset))
	{
		fread(&readData, sizeof(struct data), 1, fp);
	}
	
	return readData;
}

int DataReadToPtr(FILE *fp, size_t offset, struct data *current)
{
	if (SafeSeek(fp, offset))
	{
		fread(current, sizeof(struct data), 1, fp);
		return 1;
	}
	return 0;
}

int DataWrite(FILE *fp, size_t offset, struct data *current)
{
	if (SafeSeek(fp, offset))
	{
		fwrite(current, sizeof(struct data), 1, fp);
		return 1;
	}
	return 0;
}

struct head HeaderRead(FILE *fp)
{
	struct head header = {0};
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(struct head), 1, fp);
	return header;
}

void HeaderReadToPtr(FILE *fp, struct head *header)
{
	fseek(fp, 0, SEEK_SET);
	fread(header, sizeof(struct head), 1, fp);
}

void HeaderWrite(FILE *fp, struct head *header)
{
	fseek(fp, 0, SEEK_SET);
	fwrite(header, sizeof(struct head), 1, fp);
}

int Alloc(FILE *fp, char ch)
{
	struct head header = {0};
	struct data next_free = {0};
	struct data next = {0};
	uint next_allocated = 0; 

	HeaderReadToPtr(fp, &header);

	if (NULL_TERM == header.free)
	{
		return 0;
	}

	if (FILE_SIZE - sizeof(struct data) == header.free)
	{
		printf("\e[91mdb full\e[0m\n");
		return -1;
	}

	DBG(DEBUG, printf("attempt read\n"));

	/* read next free list values */
	if (DataReadToPtr(fp, header.free, &next))
	{
		DBG(DEBUG, printf("next success\n"));
		if (DataReadToPtr(fp, next.next, &next_free))
		{
			DBG(DEBUG, printf("next_free success\n"));
			next_allocated = header.allocated;
			header.allocated = header.free;
			header.free = next.next;
			next.next = next_allocated;
			next.ch = ch;

			DataWrite(fp, header.free, &next_free);
			DataWrite(fp, header.allocated, &next);
			HeaderWrite(fp, &header);

			return 1;
		}
	}
	
	return 0;
}

void Free(FILE *fp, char ch)
{
	/* what */
}

void ReadAndPrintChar(FILE *fp, struct data *current)
{
	putc(current->ch, stdout);
}

void RecursiveReadAndAction(FILE *fp, struct data *current, void (*action)(FILE *, struct data*))
{
	struct data next = {0};

	DBG(DEBUG, printf("<current next=%x>\n", current->next));
	if (NULL_TERM == current->next)
	{
		action(fp, current);
		return;
	}
	else if (0 == current->next)
	{
		return;
	}

	DataReadToPtr(fp, current->next, &next);

	RecursiveReadAndAction(fp, &next, action);
	DBG(DEBUG, printf("<action:current next=%x ch=%c>\n", current->next, current->ch));
	action(fp, current);
}

void ReadAndAction(FILE *fp, void (*action)(FILE *, struct data*))
{
	struct head header = {0};
	struct data current = {0};

	HeaderReadToPtr(fp, &header);

	DataReadToPtr(fp, header.allocated, &current);

	/*while (current.next != NULL_TERM)
	{
		action(fp, &current);
		DataReadToPtr(fp, current.next, &current);
	}
	*/
	RecursiveReadAndAction(fp, &current, action);
}

int CreateDb(char *filename)
{
	uint max = (FILE_SIZE - sizeof(struct head)) / sizeof(struct data);
	FILE *fp = NULL;
	struct data stub = {0};
	struct head header = {0};

	fp = fopen(filename, "wb");
	if (NULL == fp)
	{
		printf("file already created\n");
		return 1; /* file can't be edited */
	}

	header.free = sizeof(struct head);
	header.allocated = NULL_TERM;
	fwrite(&header, sizeof(struct data), 1, fp);

	for(int i = 0; max > i; ++i)
	{
		if (i < max - 1)
		{
			stub.next = sizeof(struct data) * (i + 1) + sizeof(struct head);
		}
		else
		{
			stub.next = NULL_TERM;
		}
		
		stub.ch = 0;
		fwrite(&stub, sizeof(struct data), 1, fp);
	}

	fclose(fp);
	fp = NULL;

	return 0;
}

int InsertString(char *filename, char *string, size_t len)
{
	FILE *fp = NULL;
	int didAllocSucceed = 0;
	uint string_idx = 0;

	if (NULL == string)
	{
		return 2; /* bad string */
	}

	fp = fopen(filename, "rb+");
	if (NULL == fp)
	{
		return 1; /* file can't be edited */
	}

	do 
	{
		didAllocSucceed = Alloc(fp, string[string_idx]);

		++string_idx;
	} while (len > string_idx && 0 != string[string_idx]
		     && didAllocSucceed == 1);

	if (1 == didAllocSucceed)
	{
		printf("\e[92msaving\e[0m\n");
	}
	
	DBG(DEBUG, printf("saved %x chars\n", string_idx));

	fclose(fp);
	fp = NULL;

	return 0;
}

int ReadAndPrint(char *filename)
{
	FILE *fp = NULL;

	fp = fopen(filename, "r");
	if (NULL == fp)
	{
		return 1; /* file can't be edited */
	}

	ReadAndAction(fp, ReadAndPrintChar);

	fclose(fp);
	fp = NULL;

	return 0;
}


int main(int argc, char **argv)
{
	int code = 0;
	char read[MAX_STRING_SIZE] = {0};
	char *read_data = (char *)&read;

	if (argc < 2)
	{
		printf("Usage: linked_list_char_db filename");
		return 1;
	}

	code = CreateDb(argv[1]);
	if (-1 == code)
	{
		printf("Database build failed: %s\n", argv[1]);
		return 1;
	}
	
	fgets(read_data, MAX_STRING_SIZE, stdin);
	code = InsertString(argv[1], read_data, MAX_STRING_SIZE);
	if (1 == code)
	{
		printf("Could not open file\n");
	}

	ReadAndPrint(argv[1]);

	return 0;
}