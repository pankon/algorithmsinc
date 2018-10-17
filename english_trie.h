/*
 * english_trie.h
 */

#define N_LETTERS 26

typedef struct english_trie
{
	struct english_trie *parent;
	char current;
	struct english_trie *children[N_LETTERS];
}