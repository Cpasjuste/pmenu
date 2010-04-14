
#ifndef h_pnd_container_h
#define h_pnd_container_h

#ifdef __cplusplus
extern "C" {
#endif

// this interface defines a simple container for storing unsorted data; it is likely to just be
// the worst linked list you've ever seen, but could be changed later and applications should only
// need a recompile

// this simple 'Box' (shorter to type) container is not intended to be super efficient or highly usable
// and as such does not include delete-node functionality or othe useful APIs, nor internal data to
// support such operatins yet. Should performance become important, a hash or somesuch might be
// needing implementing.

// The user may have a payload of a simple char*string, or a struct, or whatever they choose to stuff
// in there.

typedef void* pnd_box_handle;

/* return a new container with no contained nodes.
 * Returns NULL on failure.
 */
pnd_box_handle pnd_box_new ( char *name );

/* delete the container and all of its contents. Note that of course you have to free up any memory
 * referred to in advance of this operation, lest there be leaks!
 * Assume success always (ie: the container is either destroyed or indeterminate after.)
 */
void pnd_box_delete ( pnd_box_handle box );

/* allocinsert() is used to allocate a new entry in the container of the specified size; ie:
 * the 'key' is a regular char* string; the 'size' refers to the requested size of the payload
 * you need, not the size of the key.
 * Returns a new payload pointer of the requested size, or NULL. Do not free this payload pointer
 * ever by hand.
 */
void *pnd_box_allocinsert ( pnd_box_handle box, char *key, unsigned int size );

/* use find_by_key() to find a value or iteration, and unlink it */
void pnd_box_delete_node ( pnd_box_handle box, void *value );

/* find_by_key() will attempt to locate a payload based on the specified key.
 * Returns the payload pointer or NULL if not found.
 * CASE INSENSITIVE.
 */
void *pnd_box_find_by_key ( pnd_box_handle box, char *key );

/* merge two box lists */
unsigned char pnd_box_append ( pnd_box_handle box, pnd_box_handle append );

/* should the user want to walk around the container, a couple basic functions are
 * provided.
 */
char *pnd_box_get_name ( pnd_box_handle box );
void *pnd_box_get_head ( pnd_box_handle box );
char *pnd_box_get_key ( void *node );
void *pnd_box_get_next ( void *node );
unsigned int pnd_box_get_size ( pnd_box_handle box );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
