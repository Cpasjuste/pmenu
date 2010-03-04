
#ifndef h_pnd_desktop_h
#define h_pnd_desktop_h

#ifdef __cplusplus
extern "C" {
#endif

#define PND_PNDHUP_KEY "launcher.hupscript"
#define PND_PNDHUP_FILENAME "pnd_hup.sh"

// emit_dotdesktop() will determine a filename and create a FILENAME.desktop file in the targetpath
// TODO: Copy the icon into this directory as well, if its source is a .pnd or info is in the dico struct
#define PND_DOTDESKTOP_HEADER "[Desktop Entry]"
#define PND_DOTDESKTOP_SOURCE "X-Pandora-Source=libpnd"
unsigned char pnd_emit_dotdesktop ( char *targetpath, char *pndrun, pnd_disco_t *p );

// emit_icon() will attempt to copy the icon from a PXML directory, or from a pnd file if appended,
// to the given directory; returns 1 on sucess, otherwise is a fail.
unsigned char pnd_emit_icon ( char *targetpath, pnd_disco_t *p );
unsigned char *pnd_emit_icon_to_buffer ( pnd_disco_t *p, unsigned int *r_buflen ); // returns length of malloc in r_buflen if !NULL

// pnd_map_dotdesktop_categories() will attempt to find an appropriate standard .desktop category(s) based
// on the provided PXML-style category.
//   In essence, the PXML top-level (and alternate) category will be used, but if (for example) the top
// level primary cat is no good, then it will descend into the alternates for that to find the mapping. 
//   NOTE: PXML has a prime and alt category, with sub-category; the standard only supports a flat list of
// categories to show an entry in (though any number of 'alternates.' so the formats are not directly
// compatible.
//   Pass in the PXML handle, and the target buffer will be filled up
//   Returns the number of successful mappings on success, or -1 for error (0 for no matches of course.)
// QUESTION: It possible makes sense to just copy over verbatim any categories that are unmapped, as a 'best guess'
//   scenario, but it currently is not coded like that.
int pnd_map_dotdesktop_categories ( pnd_conf_handle c, char *target_buffer, unsigned short int len, pnd_disco_t *d ); // <---
char *pnd_map_dotdesktop_category ( pnd_conf_handle c, char *single_category ); // not likely needed by anyone
// this default is only used if the conf file's "default" key cannot be found, and the category cannot be mapped
#define PND_DOTDESKTOP_DEFAULT_CATEGORY "Application;Utility;"

#ifdef __cplusplus
} /* "C" */
#endif

#endif
