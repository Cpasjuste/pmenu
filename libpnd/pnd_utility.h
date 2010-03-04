
#ifndef h_pnd_utility_h
#define h_pnd_utility_h

#ifdef __cplusplus
extern "C" {
#endif

// expand_tilde() will only function correctly if a user is actually logged in; perhaps you
// want to spin until it looks like someone has in fact done so. (most devices will likely
// have auto-login, but its not instantaneous!)
//  r_username is optional; if present, will receive a copy of username
unsigned char pnd_check_login ( char *r_username, unsigned int maxlen );

// given a malloc'd pointer to a string, expand ~ to $HOME as often as it is found, returning a
// new string; the old string is destroyed in the process, or returned as-is.
char *pnd_expand_tilde ( char *freeable_buffer );

// arbtrary execute function; fire and forget really
void pnd_exec_no_wait_1 ( char *fullpath, char *arg1 );

//  NOTE: Does _NOT_ automatically pick up PXML-overrides; you can call that function if you want
pnd_pxml_handle *pnd_pxml_get_by_path ( char *fullpath );

// determine_mountpoint() will examine a path, and return the mountpoint that this path
// is sitting on; returns 1 on success, meaning the target was populated.
// consider for a similar effect: df /home -> look at "Mounted on"
unsigned char pnd_determine_mountpoint ( char *fullpath, char *r_mountpoint, unsigned char mountpoint_len );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
