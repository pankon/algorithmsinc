#ifndef __VAR_MEM_MAN_H__
#define __VAR_MEM_MAN_H__

typedef struct head
{
	size_t magic;
	unsigned int free;
	unsigned int allocated;
} head_t;

typedef struct VarMemMan
{
	FILE *fp;
	head_t header;
	size_t file_size;
} VarMemMan_t;

typedef enum
{
	ALLOC_SUCCESS,
	ALLOC_FAIL
} VARMEMMAN_STATUS;

VarMemMan_t *VarMemManCreateOrLoadDb(char *filename, size_t magic, size_t file_size);

void VarMemManDestroy(VarMemMan_t *mem_man);

void *VarMemManAlloc(VarMemMan_t *mem_man, int size);

void VarMemManCommit(VarMemMan_t *mem_man, void *data);

void VarMemManFree(VarMemMan_t *mem_man, void *data);
void VarMemManDelete(VarMemMan_t *mem_man, void *data);
void VarMemManDeleteWipe(VarMemMan_t *mem_man, void *data, int wipe);

#endif /* __VAR_MEM_MAN_H__ */