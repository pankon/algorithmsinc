/*
 * Patricia trie dictionary
 * Nathan Pankowsky
 * based on design by D. R. Morrison
 * and discussion by Robert Sedgewick
 */

#ifndef __PATRICIA_H__
#define __PATRICIA_H__

#include <sys/types.h>

typedef struct patricia
{
    char *key;
    size_t str_len;
    size_t idx_bit_diff;
    struct patricia *children[2];
    void *data;
} patricia_t;

patricia_t *PatriciaCreate();
void PatriciaDestroy(patricia_t *patricia);

int PatriciaAdd(patricia_t *patricia, unsigned char *key, 
                size_t str_len, void *data);

void *PatriciaFind(patricia_t *patricia, unsigned char *key, 
                   size_t str_len);

int PatriciaDelete(patricia_t *patricia, unsigned char *key, 
                   size_t str_len);

#endif /* __PATRICIA_H__ */