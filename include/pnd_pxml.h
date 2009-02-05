
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
char *pnd_pxml_get_icon_path ( pnd_pxml_handle h );
char *pnd_pxml_get_unique_id ( pnd_pxml_handle h );
char *pnd_pxml_get_primary_category ( pnd_pxml_handle h );
char *pnd_pxml_get_exec_path ( pnd_pxml_handle h );
char *pnd_pxml_get_clockspeed ( pnd_pxml_handle h );

// for 'set' functions, pass NULL value to delete existing value without setting new one
void pnd_pxml_set_app_name ( pnd_pxml_handle h, char *v );

/* utilities
 */
unsigned char pnd_is_pxml_valid_app ( pnd_pxml_handle h ); // returns 1 when pxml seems like a valid application

typedef struct
{
	char *title_en;
	char *title_de;
	char *title_it;
	char *title_fr;
	char *unique_id;
	char *standalone;
	char *icon;
	char *description_en;
	char *description_de;
	char *description_it;
	char *description_fr;
	char *previewpic1;
	char *previewpic2;
	char *author_name;
	char *author_website;
	char *version_major;
	char *version_minor;
	char *version_release;
	char *version_build;
	char *exec;
	char *main_category;
	char *subcategory1;
	char *subcategory2;
	char *altcategory;
	char *altsubcategory1;
	char *altsubcategory2;
	char *osversion_major;
	char *osversion_minor;
	char *osversion_release;
	char *osversion_build;
	char *associationitem1_name;
	char *associationitem1_filetype;
	char *associationitem1_parameter;
	char *associationitem2_name;
	char *associationitem2_filetype;
	char *associationitem2_parameter;
	char *associationitem3_name;
	char *associationitem3_filetype;
	char *associationitem3_parameter;
	char *clockspeed;
	char *background;
	char *startdir;

}  pnd_pxml_t;

#ifdef __cplusplus
} /* "C" */
#endif

#endif
