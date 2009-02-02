
#ifndef h_pnd_pxml_h
#define h_pnd_pxml_h

#ifdef __cplusplus
extern "C" {
#endif

// this code is for very basic PXML.xml file parsing

#define PXML_FILENAME "PXML.xml" /* a specification defined name */

// use this handle to interact with PXML; this hides the mechanics of parsing a PXML file so that
// it can be upgraded with impacting applications
typedef void* pnd_pxml_handle;

/* pxml_fetch() will return NULL on fail, otherwise a valid handle which may be further queried
 */
pnd_pxml_handle pnd_pxml_fetch ( char *fullpath );
void pnd_pxml_delete ( pnd_pxml_handle h );

/* overrides() allow for customization of a PXML that persists; ie: An application might be sitting
 * on an SD card and popped out while we need to edit its personalized category; more to point, the
 * PXML itself could be in a read-only SD or packed into an ISO, or just sitting in a directory.
 * Rather than have a _second_ PXML in the same place or have to write back to read-only media or
 * worry about losing customizations when an app is temporarily deleted, we can just keep the
 * overrides themselves in NAND.
 */
/* merge_override() will attempt to locate an override of the given PXML, and will modify the
 * PXML in-place to include any overrides found.
 * Returns >0 if a merge was done, 0 if no merge was done, and <0 on error
 * NOTE: For searchpath, should query configs for PND_PXML_OVERRIDES_KEY (or use PND_PXML_OVERRIDES_SEARCHPATH)
 */
signed char pnd_pxml_merge_override ( pnd_pxml_handle h, char *searchpath );

/* these accessor functions will return READ ONLY char*s; do not free them or modify them.
 */
char *pnd_pxml_get_app_name ( pnd_pxml_handle h );
char *pnd_pxml_get_unique_id ( pnd_pxml_handle h );
char *pnd_pxml_get_primary_category ( pnd_pxml_handle h );
char *pnd_pxml_get_exec_path ( pnd_pxml_handle h );
char *pnd_pxml_get_icon ( pnd_pxml_handle h );

// for 'set' functions, pass NULL value to delete existing value without setting new one
void pnd_pxml_set_app_name ( pnd_pxml_handle h, char *v );

/* utilities
 */
unsigned char pnd_is_pxml_valid_app ( pnd_pxml_handle h ); // returns 1 when pxml seems like a valid application

#ifdef __cplusplus
} /* "C" */
#endif

#endif
