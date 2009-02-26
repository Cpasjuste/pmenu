
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

typedef enum {
  pnd_object_type_unknown = 0,
  pnd_object_type_directory,
  pnd_object_type_pnd,
  pnd_object_type_max
} pnd_object_type_t;

// another struct? Have always intended discovery_t to have minimal members.. just enough to lead to an
// application (PXML, xecutable, name); if the apps want more details, they can use the pnd_pxml code to
// fetch the full PXML and get all the details. But I think we got out of control here :)
typedef struct {
  // base
  unsigned char object_type;   // see enum above
  char *path_to_object;        // full path to the PXML.xml or awesomeapp.pnd file
  unsigned int pnd_icon_pos;   // offset to the byte after end of PXML in a pnd file (should be icon if present)
  // strdup'd from PXML
  char *title_en;
  char *unique_id;
  char *icon;
  char *exec;
  char *main_category;
  char *clockspeed;
  char *startdir;
} pnd_disco_t;

void pnd_disco_destroy ( pnd_disco_t *p ); // a function name that simply could not be avoided

// emit_dotdesktop() will determine a filename and create a FILENAME.desktop file in the targetpath
// TODO: Copy the icon into this directory as well, if its source is a .pnd or info is in the dico struct
unsigned char pnd_emit_dotdesktop ( char *targetpath, char *pndrun, pnd_disco_t *p );

// emit_icon() will attempt to copy the icon from a PXML directory, or from a pnd file if appended,
// to the given directory; returns 1 on sucess, otherwise is a fail.
unsigned char pnd_emit_icon ( char *targetpath, pnd_disco_t *p );

// TODO: A way to release the disco-lists and reclaim RAM :)

#ifdef __cplusplus
} /* "C" */
#endif

#endif
