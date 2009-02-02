
#ifndef h_pnd_apps_h
#define h_pnd_apps_h

#ifdef __cplusplus
extern "C" {
#endif

#define PND_APPS_SEARCHPATH "/mnt/sd1/pandora/apps:/mnt/sd2/pandora/apps:./testdata/apps"
#define PND_APPS_KEY "autodiscovery.searchpath"

#define PND_PNDRUN_SEARCHPATH "pnd.searchpath"
#define PND_PNDRUN_KEY "pnd.default"
#define PND_PNDRUN_FILENAME "pnd_run.sh"
#define PND_PNDRUN_DEFAULT "./testdata/scripts/pnd_run.sh"

#define PND_PXML_OVERRIDE_SEARCHPATH "~/pxml-overrides"
#define PND_PXML_OVERRIDE_KEY "overrides.searchpath"

#define PND_MOUNT_PATH "/mnt/apps/" /* all mounted PND images should be here.. /mnt/apps/myapp/... */

/* pnd_apps_exec() is used to blindly launch an app, be it a .pnd file bundle or a plain executable
 * (shell, bin, whatever.) pndrun specifies the full path to the pnd_run sh script, which should be
 * found using searchpaths and locates.. see locatetest.c for a sample
 *   if fullpath ends in PXML_FILENAME (PXML.xml) then pnd_run will be invoked (after extracting goodies
 *     from PXML file as appropriate)
 *   if fullpath ends in .pnd then pnd_run will be invoked (after inspecting embedded PXML or best-guess)
 *   otherwise the fullpath will be executed as-is
 */
signed char pnd_apps_exec ( char *fullpath, char *pndrun );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
