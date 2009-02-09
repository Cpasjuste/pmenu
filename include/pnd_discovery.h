
#ifndef h_pnd_discovery_h
#define h_pnd_discovery_h

#ifdef __cplusplus
extern "C" {
#endif

// this code is for doing the application discovery against a given directory and its children (ie: this
// code could be called on appliction startup, or as a result of media eject/insert, and so forth.)

/* disco_search() will walk the given search path (one or more paths, colon separated) in order left to
 * right; as PXML's are found, basic analysis is performed to verfy validity. A list of valid applications
 * is returned, the union of all matches in the search path
 * If no matches are found, NULL is returned (to save you deleting the container)
 * overridespath may be NULL if you do not wish to search for pxml overrides
 */
pnd_box_handle pnd_disco_search ( char *searchpath, char *overridespath );

/* pnd_disco_t describes a given entry found by the discovery code; ie: the containers key is the path to
 * the PXML file (since this is relatively unique), with the fields below detailing the executable path,
 * application name, unique-id and so on
 *
 * NOTE: this struct is dynamicly populated; you are responsible for invoking the destroy() function to
 * kill its contents, and the pnt_box_destroy() to kill the container at the end
 *
 * NOTE: The PXML path (or .pnd file path in bundle files) is used as a key since it is unique. The
 * application 'unique id' will be unique for a given app, but that app could be in multiple directories
 * or on multiple SD cards or wifi or whatever, so only the mounted path is truly unique. The key
 * is only used internally so the consumer can refer to multiple versions of the same app without
 * confusion.. it is not displayed. So no big deal.
 */

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

} pnd_disco_t;

void pnd_disco_destroy ( pnd_disco_t *p ); // a function name that simply could not be avoided

#ifdef __cplusplus
} /* "C" */
#endif

#endif
