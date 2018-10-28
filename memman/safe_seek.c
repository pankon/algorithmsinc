
#include "safe_seek.h"

SEEK_STATUS Seek(FILE *fp, size_t offset, size_t null_term)
{
	if (null_term <= offset)
	{
		return (SEEK_FAIL); /* failure */
	}

	fseek(fp, offset, SEEK_SET);

	return (SEEK_SUCCESS);
}