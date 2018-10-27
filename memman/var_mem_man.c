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

void *VarMemManAlloc(VarMemMan_t *mem_man, int size)
{
    data_t last_free = {0};
    data_t current_free = {0};
    data_t current_allocated = {0};
    data_t new_allocated = {0};
    unsigned int last_last_free_offset = 0;
    unsigned int last_free_offset = 0;
    unsigned int current_allocated_offset = 0;
    unsigned int new_free_offset = 0;
    user_data_t *user_data = NULL;
    int split_size = 0;

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

    PrintVarMemMan(mem_man);

    last_free_offset = mem_man->header.free;
    last_free.next = NULL_TERM;
    IoRead(mem_man->fp, mem_man->header.free, &current_free, sizeof(data_t), NULL_TERM);

    printf("current free: ");
    PrintDataSegment(&current_free);

    while (current_free.size < size && NULL_TERM != current_free.next)
    {
        if (current_free.size < size)
        {
            printf("this chunk is too small! %x\n", current_free.size);
        }

        did_update_in_middle = 1;
        last_free_offset = last_free.next;
        printf("last_free: "); PrintDataSegment(&last_free);
        last_free = current_free;
        
        if (READ_SUCCESS != IoRead(mem_man->fp, last_free.next, &current_free, 
               sizeof(data_t), NULL_TERM))
        {
            printf("could not allocate\n");
            return (NULL);
        }

        printf("current free: ");
        PrintDataSegment(&current_free);
    }

    if (current_free.size < size && NULL_TERM == current_free.next)
    {
        fprintf(stderr, "Database full\n");
        return (NULL);
    }

    split_size = current_free.size - (sizeof(data_t) + size);
    /* split memory if large enough, otherwise return whole block */
    if (split_size > 0)
    {
        printf("doing split into: %x & %x\n", size, split_size);
        did_split = 1;

        /* update current free size after split */
        current_free.size = split_size;
        new_allocated.size = size;
        new_allocated.is_allocated = 1;
        new_allocated.next = mem_man->header.allocated;

        /* TODO: Update last_size, with merging */
        new_free_offset = last_free_offset + sizeof(data_t) + size;
    }
    else
    {
        fprintf(stderr, "Not splitting\n");
        current_free.is_allocated = 1;
        new_allocated = current_free;

        new_free_offset = last_free.next;
    }

    /* save current allocated and set new value */
    current_allocated_offset = mem_man->header.allocated;
    mem_man->header.allocated = last_free_offset;
    new_allocated.next = current_allocated_offset;

    /* "delete" current_free */
    if (did_update_in_middle)
    {
        printf("moving last_free.next = %x from %x\n", last_free.next, current_free.next);
        last_last_free_offset = last_free.next;
        last_free.next = current_free.next;
        mem_man->header.free = last_last_free_offset;
    }
    else
    {
        fprintf(stderr, "updating header.free; %x\n", last_free_offset);
        mem_man->header.free = new_free_offset;
    }

    if (did_split)
    {
        if (WRITE_SUCCESS != IoWrite(mem_man->fp, new_free_offset, &current_free, 
                                     sizeof(data_t), NULL_TERM))
        {
            fprintf(stderr, "Split write error\n");
            return (NULL);
        }   
    }
   
    if (WRITE_SUCCESS != VarMemManWriteHeader(mem_man))
    {
        return (NULL);
    }

    IoWrite(mem_man->fp, last_free_offset, &last_free, 
        sizeof(data_t), NULL_TERM);

    if (WRITE_SUCCESS != IoWrite(mem_man->fp, mem_man->header.allocated, &new_allocated, 
        sizeof(data_t), NULL_TERM))
    {
        return (NULL);
    }

    PrintDataSegment(&new_allocated);
    PrintVarMemMan(mem_man);

    if (NULL == (user_data = malloc(sizeof(user_data_t) + size)))
    {
        return (NULL);
    }

    user_data->offset = mem_man->header.allocated;
    user_data->size = size;

    return (user_data->data);
}

void VarMemManCommit(VarMemMan_t *mem_man, void *data)
{
    user_data_t *user_data = NULL;
    if (NULL == mem_man || data == NULL)
    {
        return;
    }

    user_data = (void *)((size_t)data - (sizeof(unsigned int) * 2));

    IoWrite(mem_man->fp, user_data->offset + sizeof(data_t), 
            user_data->data, user_data->size, NULL_TERM);
}

static user_data_t *VarMemManGetUserDataFromPtr(void *data)
{
    if (data == NULL)
    {
        return ((user_data_t *)NULL);
    }

    return ((user_data_t *)((size_t)data - (sizeof(unsigned int) * 2)));
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

