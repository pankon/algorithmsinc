/*
 *
 */

#ifndef __SAFE_IO_H__
#define __SAFE_IO_H__

#include <stdio.h> /* fread, fwrite */

typedef enum
{
	READ_SUCCESS,
	READ_FAIL,
	WRITE_SUCCESS,
	WRITE_FAIL,
	WRITE_WAIT,
	FILE_EXISTS,
	FILE_DOES_NOT_EXIST
} IO_STATUS;


IO_STATUS IoRead(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term);

IO_STATUS IoWrite(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term);

IO_STATUS IoDoesFileExist(char *filename);


#endif /* __SAFE_IO_H__ */