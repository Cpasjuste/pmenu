
#ifndef h_pnd_apps_h
#define h_pnd_apps_h

#ifdef __cplusplus
extern "C" {
#endif

#include "pnd_container.h"
#include "pnd_discovery.h"

// default application searchpath (and key to look it up in config)
#define PND_APPS_SEARCHPATH "/media/*/pandora/apps:/usr/pandora/apps"
#define PND_APPS_KEY "autodiscovery.searchpath"

#define PND_APPS_APPDATA_PREFIX "/pandora/appdata" /* /media/DEVNAME + PREFIX + unique-id */

// default notify searchpath (and key to look it up in config)
#define PND_APPS_NOTIFYPATH "/media:/media/*/pandora/apps:/usr/pandora/apps"
#define PND_APPS_NOTIFY_KEY "autodiscovery.notifypath"

#define PND_PNDRUN_SEARCHPATH_KEY "pnd.searchpath"
#define PND_PNDRUN_KEY "pnd.runscript"
#define PND_PNDRUN_FILENAME "pnd_run.sh"
#define PND_PNDRUN_DEFAULT "/usr/pandora/scripts/pnd_run.sh"

#define PND_PXML_OVERRIDE_SEARCHPATH "~/pxml-overrides"
#define PND_PXML_OVERRIDE_KEY "overrides.searchpath"

#define PND_MOUNT_PATH "/mnt/pnd/" /* all mounted PND images should be here.. /mnt/apps/UNIQUE-ID/... */

// .desktop support
#define PND_DESKTOP_DOTDESKTOP_PATH_KEY "desktop.dotdesktoppath"
#define PND_DESKTOP_ICONS_PATH_KEY "desktop.iconpath"
#define PND_DESKTOP_SEARCH_KEY "desktop.searchpath"

#define PND_MENU_DOTDESKTOP_PATH_KEY "menu.dotdesktoppath"
#define PND_MENU_ICONS_PATH_KEY "menu.iconpath"
#define PND_MENU_SEARCH_KEY "menu.searchpath"

#define PND_DESKTOP_DOTDESKTOP_PATH_DEFAULT "~/.applications"
#define PND_DESKTOP_ICONS_PATH_DEFAULT "~/.applications"
#define PND_DESKTOP_SEARCH_PATH_DEFAULT "/media/*/pandora/desktop:/usr/pandora/apps"

// apps
#define PND_DEFAULT_WORKDIR "./"

/* pnd_apps_exec() is used to blindly launch an app, be it a .pnd file bundle or a plain executable
 * (shell, bin, whatever.) pndrun specifies the full path to the pnd_run sh script, which should be
 * found using searchpaths and locates.. see locatetest.c for a sample
 * pnd_run, fullpath, unique_id, rel_exec required
 * rel_startdir, clockspeed, options are optional
 * NOTE: Use pnd_locate function to locate the pnd_run, for example
 * NOTE: if specified, clock speed will be set prior to invoking the script, and set back on exit
 * NOTE: No values can be except clockspeed; a 0 clockspeed means 'leave alone'. Set startdoir to "." instead of NULL.
 * fork() is implied; calling this function does not kill this process :)
 * NOTE: Pass in the full path to the awesomeapp.pnd or to the directory containing PXML.xml (not the PXML.xml itself.)
 * Options is a set of boolean flags, derived from the #define's below; OR them together.
 *   option-block, when set, suggests the launch should wait until the invoked application exits (disregarding why app exits)
 *   example: options = PND_EXEC_OPTION_BLOCK | PND_EXEC_OPTION_2;
 */
#define PND_EXEC_OPTION_NIL        0
#define PND_EXEC_OPTION_BLOCK      1 /* wait till children complete; note, children might fork on their own.. */
#define PND_EXEC_OPTION_NOUNION    2 /* request pnd_run not use a union, just do the mount/run */
#define PND_EXEC_OPTION_NOX11      4 /* request pnd_run to kill x11 and restart it after */
#define PND_EXEC_OPTION_NORUN      8 /* don't try to run; just form the pnd_run.sh line and cache it */
#define PND_EXEC_OPTION_FUTURE2   16

unsigned char pnd_apps_exec ( char *pndrun, char *fullpath, char *unique_id,
			      char *rel_exec, char *rel_startdir,
			      char *args,
			      unsigned int clockspeed, unsigned int options );
char *pnd_apps_exec_runline ( void ); // returns the cached pnd_run.sh line from last PND_EXEC_OPTION_NORUN

// this is a superior version of pnd_apps_exec(), but avoiding breaking the pnd_apps_exec() API to
// extend it.
// - same option booleans
// - 'reserved' should be NULL unless you know what to put there (depends on options)
// - 'app' should be a return from discovery, or a populated disco-t struct
// - 'pndrun' is a reference to a pnd_run.sh script, to avoid seeking it out every time
unsigned char pnd_apps_exec_disco ( char *pndrun, pnd_disco_t *app, unsigned int options, void *reserved );

// should you wish to know where an app will get mounted, call this function to obtain a guess. The
// logic is wrapped up in pnd_run.sh, but in theory should be easily determined.

// get_appdata_path() is the one you probably want.. the appdata path (which includes both the
// files in the pnd, and any files updated/written-out from that app. Look up aufs or union-filesystems.)
// ie: appdata path is read/write, while ro_mountpoint is read-only
// Returns >0 on success, and if not-NULL will fill r_path (up to path_len length.)
unsigned char pnd_get_appdata_path ( char *fullpath, char *unique_id, char *r_path, unsigned int path_len );
// get_ro_mountpoint() returns the _read only_ mountpoint, where the dir or .pnd is actually
// mounted to. This is probably NOT WHAT YOU WANT. You probably want the read/write mountpoint, which
// is the union-filesystem version of it.. see pnd_get_appdata_path()
//   r_mountpoint (if !NULL) will be populated; mountpoint_len should specify the maxlen of the buffer
void pnd_get_ro_mountpoint ( char *fullpath, char *unique_id, char *r_mountpoint, unsigned int mountpoint_len );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
