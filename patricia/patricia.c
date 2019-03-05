/*
 * Patricia trie dictionary
 * Nathan Pankowsky
 * based on design by D. R. Morrison
 * and discussion by Robert Sedgewick
 */

#include <stdlib.h> /* malloc, free */
#include <stdio.h> /* size_t */
#include <string.h> /* memcmp */

#include "bits.h"
#include "patricia.h"

static void PatriciaInit(patricia_t *patricia, char *key, size_t str_len, 
                         size_t idx_bit_diff, void *data)
{
    patricia->key = key;
    patricia->str_len = str_len;
    patricia->idx_bit_diff = idx_bit_diff;
    patricia->children[0] = patricia;
    patricia->children[1] = patricia;
    patricia->data = data;
}

static patricia_t *PatriciaCreateInstance(char *key, size_t str_len, 
                                          size_t idx_bit_diff, void *data)
{
    patricia_t *patricia = NULL;

    if (NULL == (patricia = malloc(sizeof(patricia_t))))
    {
        return (NULL);
    }

    PatriciaInit(patricia, key, str_len, idx_bit_diff, data);

    return (patricia);
}

patricia_t *PatriciaCreate()
{
    return (PatriciaCreateInstance(NULL, 0, 0, NULL));
}

void PatriciaDestroy(patricia_t *patricia)
{
    /* make sure you don't delete yourself. */
}

int PatriciaAdd(patricia_t *patricia, unsigned char *key, size_t str_len, void *data)
{
    size_t idx_bit_diff = 0;

    if (NULL == patricia)
    {
        return (-1);
    }
    
    if (NULL == patricia->key)
    {
        /* empty tree */
        PatriciaInit(patricia, key, str_len, 0, data);
        return (0);
    }

    idx_bit_diff = BitDiff(patricia->key, patricia->str_len, key, str_len);
    if (BitGet(key, idx_bit_diff, str_len))
    {

    }
    else
    {

    }
}

void *PatriciaFind(patricia_t *patricia, unsigned char *key, size_t str_len)
{

}

int PatriciaDelete(patricia_t *patricia, unsigned char *key, size_t str_len)
{

}
