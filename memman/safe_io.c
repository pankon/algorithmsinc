/*
 *
 */

#include <stdio.h> /* fread, fwrite */

#include "platform.h"

#ifdef PREDEF_PLATFORM_UNIX
#include <unistd.h> /* access, F_OK - check if file exists */
#endif

#ifdef PREDEF_PLATFORM_WINDOWS
#include <windows.h>
#endif

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
#ifdef PREDEF_PLATFORM_UNIX
	if (-1 != access(filename, F_OK))
#endif
#ifdef PREDEF_PLATFORM_WINDOWS
	/* https://stackoverflow.com/questions/3828835/how-can-we-check-if-a-file-exists-or-not-using-win32-program */
	DWORD dwFileAttributes = GetFileAttributesA(filename);
	
	if (INVALID_FILE_ATTRIBUTES != dwFileAttributes 
		&& !(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
#endif
	{
		return (FILE_EXISTS);
	}

	return (FILE_DOES_NOT_EXIST);
}