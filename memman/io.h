/*
 *
 */

#ifndef __IO_H__
#define __IO_H__

typedef enum
{
	READ_SUCCESS,
	READ_FAIL,
	WRITE_SUCCESS,
	WRITE_FAIL,
	WRITE_WAIT
} IO_STATUS;


IO_STATUS IoRead(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term);

IO_STATUS IoWrite(FILE *fp, size_t offset, void *memory, size_t size, size_t null_term);

#endif /* __IO_H__ */