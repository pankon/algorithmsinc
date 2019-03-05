/*
 * basic bits operations
 * Nathan Pankowsky
 */

#include <sys/types.h>

#ifndef __BITS_H__
#define __BITS_H__

size_t BitDiff(unsigned char *a, size_t a_len, 
               unsigned char *b, size_t b_len);

int BitGet(unsigned char *a, size_t offset, size_t length);

#endif /* __BITS_H__ */