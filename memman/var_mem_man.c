/*
 * Generalized ll database variable size
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* access */

#include "io.h"
#include "var_mem_man.h"

#define NULL_TERM 0xffffffff

typedef struct data_segment
{
    int size;
    unsigned int next;
    
    unsigned char is_allocated;

    /*
    unsigned int last_size;
    unsigned char is_in_use;
    unsigned short flags;
    */
    char data[1];
} data_t;

typedef struct user_data
{
    unsigned int offset;
    unsigned int size;
    char data[1];
} user_data_t;

IO_STATUS VarMemManReadHeader(VarMemMan_t *mem_man);

void PrintVarMemMan(VarMemMan_t *mem_man)
{
    if (NULL == mem_man)
    {
        printf("No mem_man\n");
        return;
    }

    printf("mem_man = { head : \n    { magic: %x, free: %x, allocated: %x }\n}\n", 
        mem_man->header.magic, mem_man->header.free, mem_man->header.allocated);
}

void PrintDataSegment(data_t *data)
{
    if (NULL == data)
    {
        printf("No data\n");
    }

    printf("data = { size : %x, next : %x, is_allocated: %x }\n", 
           data->size, data->next, data->is_allocated);
}

VarMemMan_t *VarMemManCreateOrLoadDb(char *filename, size_t magic, size_t file_size)
{
    FILE *fp = NULL;
    VarMemMan_t *mem_man = NULL;
    head_t header = {0};
    data_t stub_data = {0};
    int stub = 0;

    if (NULL == (mem_man = malloc(sizeof(VarMemMan_t))))
    {
        printf("failed to alloc mem_man");
        return (NULL);
    }

    mem_man->fp = NULL;
    

    if (-1 != access(filename, F_OK))
    {
        printf("opening existing\n");
        fp = fopen(filename, "rb+");

        if (NULL == fp)
        {
            printf("could not open file\n");
            return (NULL); /* file can't be edited */
        }

        fseek(fp, 0L, SEEK_END);
        mem_man->file_size = ftell(fp);
        mem_man->fp = fp;

        VarMemManReadHeader(mem_man);

        if (mem_man->header.magic != magic)
        {
            VarMemManDestroy(mem_man);
            fprintf(stderr, "magic %x != %x, exiting\n", mem_man->header.magic, magic);
            return (NULL);
        }
    }
    else
    {
        fp = fopen(filename, "wb+");
        if (NULL == fp)
        {
            printf("could not create file\n");
            return (NULL); /* file can't be edited */
        }

        header.magic = magic;
        header.free = sizeof(head_t);
        header.allocated = NULL_TERM;

        mem_man->file_size = file_size;
        mem_man->fp = fp;
        mem_man->header = header;

        if (WRITE_SUCCESS != IoWrite(fp, 0, &header, sizeof(head_t), NULL_TERM))
        {
            printf("failed to write header : %d", NULL_TERM);
            return (NULL);
        }

        stub_data.size = file_size - sizeof(head_t) - sizeof(data_t);
        stub_data.next = NULL_TERM;
        
        if (WRITE_SUCCESS != IoWrite(fp, sizeof(head_t), &stub_data, 
                                     sizeof(data_t), NULL_TERM))
        {
            printf("failed to write header : %d", NULL_TERM);
            return (NULL);
        }

        /* expand the file */
        if (WRITE_SUCCESS != IoWrite(fp, file_size - sizeof(int), &stub, sizeof(int), NULL_TERM))
        {
            printf("failed to expand file");
            VarMemManDestroy(mem_man);
            mem_man = NULL;
            return (NULL);
        }
    }
    
    return (mem_man);
}

void VarMemManDestroy(VarMemMan_t *mem_man)
{
    if (NULL != mem_man)
    {
        fclose(mem_man->fp);
        mem_man->fp = NULL;
        free(mem_man);
        mem_man = NULL;
    }
}

IO_STATUS VarMemManReadHeader(VarMemMan_t *mem_man)
{
    if (NULL == mem_man)
    {
        return READ_FAIL;
    }

    return (IoRead(mem_man->fp, 0, &(mem_man->header), sizeof(head_t), NULL_TERM));
}

IO_STATUS VarMemManWriteHeader(VarMemMan_t *mem_man)
{
    if (NULL == mem_man)
    {
        return READ_FAIL;
    }

    return (IoWrite(mem_man->fp, 0, &(mem_man->header), sizeof(head_t), NULL_TERM));
}

void SwapInt(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void SwapData(data_t *a, data_t *b)
{
    data_t temp = *a;
    *a = *b;
    *b = temp;
}

VARMEMMAN_STATUS VarMemManFindNextFree(VarMemMan_t *mem_man, int size, 
                             unsigned int *offset,  data_t *current_free, 
                             int *did_update_in_middle)
{
    if (NULL == mem_man || NULL == offset || NULL == current_free 
        || NULL == did_update_in_middle)
    {
        return (ALLOC_FAIL);
    }

    *did_update_in_middle = 0;
    *offset = mem_man->header.free;
    IoRead(mem_man->fp, mem_man->header.free, current_free, sizeof(data_t), NULL_TERM);

    while (current_free->size < size && NULL_TERM != current_free->next)
    {
        *offset = current_free->next;
        *did_update_in_middle = 1;
        
        if (READ_SUCCESS != IoRead(mem_man->fp, *offset, current_free, 
               sizeof(data_t), NULL_TERM))
        {
            return (ALLOC_FAIL);
        }
    }

    if (NULL_TERM == current_free->next && current_free->size < size)
    {
        return (ALLOC_FAIL);
    }

    return (ALLOC_SUCCESS);
}


void *VarMemManUserDataCreate(unsigned int offset, int size)
{
    user_data_t *user_data = NULL;

    if (NULL == (user_data = malloc(sizeof(user_data_t) + size)))
    {
        return (NULL);
    }

    user_data->offset = offset;
    user_data->size = size;

    return (user_data->data);
}

IO_STATUS VarMemManWriteData(VarMemMan_t *mem_man, unsigned int offset, data_t *data)
{
    /* your responsibility to null check */
    return (IoWrite(mem_man->fp, offset, data, sizeof(data_t), NULL_TERM));
}

VARMEMMAN_STATUS VarMemManSplitMemory(VarMemMan_t *mem_man, unsigned int offset, int size, 
                                      data_t *current_free)
{
    data_t new_free = {0};
    int new_free_offset = 0;
    int split_size = 0;

    printf("[VarMemManSplitMemory]\n");

    if (NULL == mem_man || NULL == current_free)
    {
        return (ALLOC_FAIL);
    }

    current_free->is_allocated = 1;

    split_size = current_free->size - (size + sizeof(data_t));
    current_free->size = size;
    
    if (split_size <= 0)
    {
        printf("not splitting\n");
        /* set header.free to current.next, and current.next to header.allocated */
        mem_man->header.free = current_free->next;
        current_free->next = mem_man->header.allocated;
        mem_man->header.allocated = offset;

        if (WRITE_SUCCESS != VarMemManWriteData(mem_man, offset, current_free))
        {
            return (ALLOC_FAIL);
        }

        if (WRITE_SUCCESS != VarMemManWriteHeader(mem_man))
        {
            return (ALLOC_FAIL);
        }

        return (ALLOC_SUCCESS);
    }

    printf("Splitting\n");

    new_free_offset = offset + size + sizeof(data_t);

    /* set new_free size to split_size, and current_free size to size */
    new_free.size = split_size;
    new_free.next = mem_man->header.free;

    /* set header.free to new_allocated offset*/
    mem_man->header.free = new_free_offset;
    current_free->next = mem_man->header.allocated;
    mem_man->header.allocated = offset;

    if (WRITE_SUCCESS != VarMemManWriteData(mem_man, offset, current_free))
    {
        return (ALLOC_FAIL);
    }

    if (WRITE_SUCCESS != VarMemManWriteData(mem_man, new_free_offset, &new_free))
    {
        return (ALLOC_FAIL);
    }

    if (WRITE_SUCCESS != VarMemManWriteHeader(mem_man))
    {
        return (ALLOC_FAIL);
    }

    return (ALLOC_SUCCESS);
}

void *VarMemManAlloc(VarMemMan_t *mem_man, int size)
{
    data_t found_free = {0};

    unsigned int offset = 0;

    int did_update_in_middle = 0;
    int did_split = 0;

    if (NULL == mem_man)
    {
        return (NULL);
    }

    if (READ_FAIL == VarMemManReadHeader(mem_man))
    {
        return (NULL);
    }

    if (ALLOC_FAIL == VarMemManFindNextFree(mem_man, size, &offset,
                                            &found_free, &did_update_in_middle))
    {
        return (NULL);
    }
  
    if (ALLOC_FAIL == VarMemManSplitMemory(mem_man, offset, size, &found_free))
    {
        return (NULL);
    }

    return (VarMemManUserDataCreate(offset, size));
}

static user_data_t *VarMemManGetUserDataFromPtr(void *data)
{
    if (data == NULL)
    {
        return ((user_data_t *)NULL);
    }

    return ((user_data_t *)((size_t)data - (sizeof(unsigned int) * 2)));
}

void VarMemManCommit(VarMemMan_t *mem_man, void *data)
{
    user_data_t *user_data = NULL;
    if (NULL == mem_man || data == NULL)
    {
        return;
    }

    user_data = VarMemManGetUserDataFromPtr(data);

    IoWrite(mem_man->fp, user_data->offset + sizeof(data_t), 
            user_data->data, user_data->size, NULL_TERM);
}

void VarMemManUpdate(VarMemMan_t *mem_man, void *data)
{
    user_data_t *user_data = NULL;
    if (NULL == mem_man || data == NULL)
    {
        return;
    }

    user_data = VarMemManGetUserDataFromPtr(data);

    IoRead(mem_man->fp, user_data->offset + sizeof(data_t), 
            user_data->data, user_data->size, NULL_TERM);
}



void VarMemManFree(VarMemMan_t *mem_man, void *data)
{
    user_data_t *user_data = NULL;
    if (mem_man == NULL || data == NULL)
    {
        return;
    }

    user_data = VarMemManGetUserDataFromPtr(data);

    free(user_data);
    user_data = NULL;
}

void VarMemManDeleteWipe(VarMemMan_t *mem_man, void *data, int wipe)
{
    data_t current_allocated = {0};
    user_data_t *user_data = NULL;
    if (mem_man == NULL || data == NULL)
    {
        return;
    }

    user_data = VarMemManGetUserDataFromPtr(data);

    if (READ_FAIL == VarMemManReadHeader(mem_man))
    {
        return;
    }

    IoRead(mem_man->fp, user_data->offset, &current_allocated, 
           sizeof(data_t), NULL_TERM);

    mem_man->header.allocated = current_allocated.next;

    current_allocated.next = mem_man->header.free;
    mem_man->header.free = user_data->offset;

    current_allocated.is_allocated = 0;

    IoWrite(mem_man->fp, user_data->offset, &current_allocated, 
           sizeof(data_t), NULL_TERM);

    if (wipe)
    {
        int stub = 0;
        int i = 0;
        for (int i = 0; user_data->size > i; ++i)
        {
            IoWrite(mem_man->fp, user_data->offset + sizeof(data_t) + i, 
               &stub, 1, NULL_TERM);
        }
    }

    VarMemManWriteHeader(mem_man);

    free(user_data);
    user_data = NULL;
}

void VarMemManDelete(VarMemMan_t *mem_man, void *data)
{
    VarMemManDeleteWipe(mem_man, data, 0);
}

