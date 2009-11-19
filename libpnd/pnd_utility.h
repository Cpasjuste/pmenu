
#ifndef h_pnd_utility_h
#define h_pnd_utility_h

#ifdef __cplusplus
extern "C" {
#endif

// given a malloc'd pointer to a string, expand ~ to $HOME as often as it is found, returning a
// new string; the old string is destroyed in the process, or returned as-is.
char *pnd_expand_tilde ( char *freeable_buffer );

// arbtrary execute function; fire and forget really
void pnd_exec_no_wait_1 ( char *fullpath, char *arg1 );

// pnd_pxml_get_always() will work against a PXML.xml file or a foo.pnd app-bundle and return
// the resulting PXML structure, or fail; ie: It should be handy to avoid the user having to
// do the 'what type is this, and if-this-or-that' themselves.
//   NOTE: Does _NOT_ automatically pick up PXML-overrides; you can call that function if you want
pnd_pxml_handle pnd_pxml_get_by_path ( char *fullpath );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
