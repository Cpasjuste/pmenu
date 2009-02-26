
#ifndef h_pnd_notify_h
#define h_pnd_notify_h

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pnd_notify_handle;

/* must invoke this to obtain a handle; other notify functions require it of course.
 * Returns NULL on failure.
 */
pnd_notify_handle pnd_notify_init ( void );

/* shutdown..
 */
void pnd_notify_shutdown ( pnd_notify_handle h );

/* register a path to watch
 * Pass in a path to register it for watching; note that should you wish children
 * directories to be watched as well you should set the recursive flag.
 */
#define PND_NOTIFY_RECURSE 1
void pnd_notify_watch_path ( pnd_notify_handle h, char *fullpath, unsigned int flags );

/* rescan_p (rescan predicate) -- wil return TRUE (>0) when the notify is reporting
 * that we should do a re-discovery against the paths.
 * NOTE:
 * (Yes this is a little brute-forcey .. it should just collect notify events
 * and suss-out the changes, and add/sub those from a working list of applications.)
 */
unsigned char pnd_notify_rediscover_p ( pnd_notify_handle h );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
