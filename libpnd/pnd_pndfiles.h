
#ifndef h_pnd_pndfiles_h
#define h_pnd_pndfiles_h

#ifdef __cplusplus
extern "C" {
#endif

// the filename of PND files must end with a well defined (case insensitive!) extension
#define PND_PACKAGE_FILEEXT ".pnd" /* case insensitive due to SD FAT fs */

// when seeking the PXML appended (or embedded within if they forgot to append it)
// to a PND file, this buffer size will be used in the seek. It'll actually under-seek
// a bit, so in the odd chance the "<PXML>" tag borders right on the window size, we'll
// still find it.
//   Being SD reads, it might be nice to pick a decent size .. SD is constant read regardless
// of read size for certain sizes, but of course strstr() within a giant buffer is no good
// either if the goods are near the end. How big is an average .png for an average icon
// size?
#define PND_PXML_WINDOW_SIZE 4096
#define PND_PXML_WINDOW_FRACTIONAL ( PND_PXML_WINDOW_SIZE - 10 )

// pnd_seek_pxml should vaguely work like fseek, trying to position at begin of the appended/found PXML
// On return of 0, assuming nothing.
// On 1, assume that the FILE pointer is positioned for next read to pull in the PXML line by line
unsigned char pnd_pnd_seek_pxml ( FILE *f );

// accrue_pxml will read through the given FILE * until it finds </PXML> (case insensitively)
// Returns 1 on success (</PXML> found), leaving file pointer after the </PXML>\n
// Returns 0 on failure (likely file pointer is at the end of the file, having sought out the </PXML>
unsigned char pnd_pnd_accrue_pxml ( FILE *f, char *target, unsigned int maxlen );

// pnd_match_binbuf will find a case insensitve string within a binary buffer (ie: strcasestr will
// only work in a non-binary buffer.) Brute force zombies ahead!
char *pnd_match_binbuf ( char *haystack, unsigned int maxlen, char *needle );

// pnd_mount() is for mounting a .pnd or PXML-app into the filesystem without running the app; for
// instance, should you want to browse screenshots (listed from PXML say?), you might want to
// pnd_mount(), lurk around in the mount, and then pnd_unmount() to release it
// On success, _mount and _unmount return >0
unsigned char pnd_pnd_mount ( char *pndrun, char *fullpath, char *unique_id );
unsigned char pnd_pnd_unmount ( char *pndrun, char *fullpath, char *unique_id );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
