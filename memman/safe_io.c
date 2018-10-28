/*
 *
 */

#include <stdio.h> /* fread, fwrite */

#include <unistd.h> /* access, F_OK - check if file exists */

#include "seek.h"
#include "safe_io.h"

IO_STATUS IoRead(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term)
{
	if (SEEK_SUCCESS == Seek(fp, offset, null_term))
	{
		fread(memory, size, 1, fp);
		return READ_SUCCESS;
	}
	return READ_FAIL;
}

IO_STATUS IoWrite(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term)
{
	if (SEEK_SUCCESS == Seek(fp, offset, null_term))
	{
		fwrite(memory, size, 1, fp);
		return (WRITE_SUCCESS);
	}
	return (WRITE_FAIL);
}

IO_STATUS IoDoesFileExist(char *filename)
{
	if (-1 != access(filename, F_OK))
	{
		return (FILE_EXISTS);
	}

	return (FILE_DOES_NOT_EXIST);
}