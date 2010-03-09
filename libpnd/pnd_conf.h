
/*
 * libpnd - tools for the Open Pandora device
 * This code to be released under the terms of the GPLv2
 */

#ifndef h_pnd_conf_h
#define h_pnd_conf_h

#ifdef __cplusplus
extern "C" {
#endif

/* Config file format
 * Since we're sticking configs in homedirs and /etc, it seems inappropriate to use XML; further
 * of course, XML means a heavy duty parser (expat or something at least) when people really just
 * use grep on these guys. Still, XML would afford us unicode which would be nice..
 * Anyway, the very basic format of the config file supported herein is derived from .ini really
 * (sorry!)
 *
 * [section]
 * key value #comment
 *
 * Comments and leading/trailing space is removed; embedded space is left.
 *
 * Thats it; internally, 'section' is prepended to all keys, so it looks something like this.
 * Section can be [] to mean 'back to the top level.' I had
 * considered having no decorations around [section] (which is to say, a key with no value) but
 * I could see cases where folks want to have a valueless key, without something goofy like value
 * 'present' 'on' '0', when they just want a key 'use-foo' and don't want 'use foo' value 'no'
 * that feals yes/no-ey.
 *
 * NOTE: If a key is present in a config twice.. well, too bad. No warning is made.
 *
 * autodiscovery:
 * path /mnt/sd1/my/path
 *
 * Internally becomes:
 *
 * autodiscovery.path -> /mnt/sd1/my/path
 *
 */

/* certain paths must be 'givens'; ideally as few assumptions as possible, but to
 * find further config files we need to look _somewhere_; note that searchpaths
 * are searched in order, so the most-canonicle should be listed last.
 *
 * NOTE: This search path is used to find the basic config files, which in turn
 * can specify a replacement config path .. it may be required to first locate the
 * 'conf' config, and then use its suggested searchpath!
 *
 */
#define PND_CONF_SEARCHPATH "/media/mmcblk0p1/pandora/conf:/media/mmcblk1p1/pandora/conf:/etc/pandora/conf:./testdata/conf"

/* within the base conf file 'conf', the key for the searchpath is 'conf.searchpath' */
#define PND_CONF_FILE       "conf" /* a config file for config settings! */
#define PND_CONF_KEY        "conf.searchpath" /* if not found, use PND_CONF_SEARCHPATH */

/* we would like to open config files based on enums, so as to minimize specifying
 * filenames; ie: It makes it easier to rename them later or display them in a native
 * language, etc. It is optional as our API will allow direct open by filename as
 * well.
 */
typedef enum {
  pnd_conf_nil = 0,
  pnd_conf_conf,           // provides settings for the config system
  pnd_conf_apps,           // provides application search-path, pxml override location, etc.
  pnd_conf_startup,        // provides list of startup applications, basic shell application, etc.
  pnd_conf_desktop,        // provides settings for the launchers
  pnd_conf_categories,     // provides mapping from PXML category to dot-desktop category
  pnd_conf_evmap,          // provides mapping from event to sh-script
} pnd_conf_filename_e;

typedef struct {
  pnd_conf_filename_e id;
  char *filename;
} pnd_conf_filename_t;

extern pnd_conf_filename_t pnd_conf_filenames[];

/* config file blackbox type
 */
typedef void* pnd_conf_handle;

/*
 * config FILE reading public API
 */

/* fetch_searchpath() - since near every app may wish to use this piece of code,
 * it is encapsulated here.
 * Returns a search-path to be used hereafter; free it when done with it!
 */
char *pnd_conf_query_searchpath ( void );

/* fetch_by_id() will try to locate the config file as referred to by the enum'd 'id'. If it
 * can be found and parsed a handle will be returned, otherwise a handle of 0 (NULL).
 * Returns a 0 handle on fail, otherwise a useful handle.
 */
pnd_conf_handle pnd_conf_fetch_by_id ( pnd_conf_filename_e id, char *searchpath );

/* if you don't wish to use an 'id', you can fetch by filename. Essentially the fetch_by_id()
 * function simply uses this to do the work, but you could use it to load up alternate config-format
 * files.
 * Returns a 0 handle on fail, otherwise a useful handle.
 */
pnd_conf_handle pnd_conf_fetch_by_name ( char *filename, char *searchpath );

/* fetch_by_path() will operate on a specific full filename; this essentially does the
 * dirty work for the above functions, but can be used to pull in a specific path for
 * whatever purpose.
 * Returns a 0 handle on fail, otherwise a useful handle.
 */
pnd_conf_handle pnd_conf_fetch_by_path ( char *fullpath );

/*
 * config file accessor functions public API
 */

/* get_as_char() will attempt to locate the specified key string (of format section.key) in the
 * provided config handle. Do not free up this value, it is considered read only.
 * Returns NULL on error, otherwise a READ ONLY char* reference to the value.
 */
char *pnd_conf_get_as_char ( pnd_conf_handle c, char *key );
#define PND_CONF_BADNUM (-31337) /* really lame hack, I know */
int pnd_conf_get_as_int ( pnd_conf_handle c, char *key );
int pnd_conf_get_as_int_d ( pnd_conf_handle c, char *key, int def ); // same as _as_int, but returns default instead of BADNUM

#ifdef __cplusplus
} /* "C" */
#endif

#endif
