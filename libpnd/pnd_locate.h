
#ifndef h_pnd_locate_h
#define h_pnd_locate_h

#ifdef __cplusplus
extern "C" {
#endif

// given a filename and a searchpath, return the first path it is found at (or NULL if not found)
// Returned path includes filename.
// WARNING: Returned path will be over-written, you must duplicate it!
char *pnd_locate_filename ( char *searchpath, char *filename );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
