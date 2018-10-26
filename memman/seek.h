/*
 *
 */

#ifndef __SEEK_H__
#define __SEEK_H__

#include <stdio.h>

typedef enum
{
	SEEK_SUCCESS,
	SEEK_FAIL
} SEEK_STATUS;

SEEK_STATUS Seek(FILE *fp, size_t offset, size_t null_term);

#endif