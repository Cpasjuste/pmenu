
#ifndef h_pnd_apps_h
#define h_pnd_apps_h

#ifdef __cplusplus
extern "C" {
#endif

#define PND_APPS_SEARCHPATH "/mnt/sd1/pandora/apps:/mnt/sd2/pandora/apps:./testdata/apps"
#define PND_APPS_KEY "autodiscovery.searchpath"

#define PND_PNDRUN_SEARCHPATH_KEY "pnd.searchpath"
#define PND_PNDRUN_KEY "pnd.runscript"
#define PND_PNDRUN_FILENAME "pnd_run.sh"
#define PND_PNDRUN_DEFAULT "./testdata/scripts/pnd_run.sh"

#define PND_PXML_OVERRIDE_SEARCHPATH "~/pxml-overrides"
#define PND_PXML_OVERRIDE_KEY "overrides.searchpath"

#define PND_MOUNT_PATH "/mnt/apps/" /* all mounted PND images should be here.. /mnt/apps/myapp/... */

// .desktop support
#define PND_DOTDESKTOP_KEY "dotfiles.dotdesktoppath"
#define PND_DOTDESKTOP_DEFAULT "/usr/share/applications"

// apps
#define PND_DEFAULT_WORKDIR "/tmp"

/* pnd_apps_exec() is used to blindly launch an app, be it a .pnd file bundle or a plain executable
 * (shell, bin, whatever.) pndrun specifies the full path to the pnd_run sh script, which should be
 * found using searchpaths and locates.. see locatetest.c for a sample
 * NOTE: Use pnd_locate function to locate the pnd_run, for example
 * NOTE: clock speed will be set prior to invoking the script, and set back on exit
 * NOTE: No values can be except clockspeed; a 0 clockspeed means 'leave alone'. Set startdoir to "." instead of NULL.
 * fork() is implied; calling this function does not kill this process :)
 */
unsigned char pnd_apps_exec ( char *pndrun, char *fullpath, char *unique_id, char *rel_exec, char *rel_startdir, unsigned int clockspeed );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
