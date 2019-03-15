/*
 * sync_node.h
 * object designed to hold a node and sync it to a file
 */

#ifndef __SYNC_NODE_H__
#define __SYNC_NODE_H__

typedef struct sync_file
{
	int handle;
} sync_file_t;

/* good example of where a builder should be used
 * if any of the functions are not supplied, just don't allow
 * the affected actions
 */
typedef struct sync_database
{
	sync_node *root;

	/* setting keys/values */
	void *(*get_key)(void *current_node, void *param);
	void *(*get_value)(void *current_node, void *param);
	int (*set_key)(void *current_node, size_t key_size, 
				   void *key, void *param);
	int (*set_value)(void *current_node, size_t value_size, 
		             void *object, void *param);

	/* iteration */
	int (*iterate)(void *current_node, void *param);

	/* in-memory management of nodes */
	void *(*create_node)(void *object, void *param);
	void *(*destroy_node)(void *object, void *param);

	/* basic operations on objects nodes point to */
	int (*add)(void *object, void *param);
	int (*remove)(void *object, void *param);

	/* doesn't have any effect on the database structure 
	 * except in optimizing search adt's
	 */
	void *(*find)(void *object, void *param);
	void *(*find_range)(void *minimum, void *min_param,
					    void *maximum, void *max_param);
	void *(*find_within)(void *minimum, void *min_param,
					     void *maximum, void *max_param);

	/* allow free of non-static data */
	int (*destroy)(void *object, void *param); 

	/* there should probably be some options in case
	 * you want to work with pipes
	 * instead of files
	 */
	char *filename;

	/* internal */
	sync_file_t sync_file;
} sync_database_t;

typedef struct sync_node
{
	void *node_object;
	size_t node_size;

	/* handled internally */
	size_t key_size;
	size_t value_size;

	/* pointer to sync operations */
	sync_database_t *database;
} sync_node_t;

#endif /* __SYNC_NODE_H__ */