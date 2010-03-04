
#ifndef h_dbusnotify_h
#define h_dbusnotify_h

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pnd_dbusnotify_handle;

/* An option is to use dbus for a lot of things, or even specific things such as
 * watching for media to be inserted or ejected; one could supplement such info
 * with inotify for file-moves.
 * Really, we need a serious model for watching and receiving system state changes
 * but I started simple, so going to try to keep it simple. For now.
 */

/* must invoke this to obtain a handle; other notify functions require it of course.
 * Returns NULL on failure.
 */
pnd_dbusnotify_handle pnd_dbusnotify_init ( void );

/* shutdown..
 */
void pnd_dbusnotify_shutdown ( pnd_dbusnotify_handle h );

/* HACK HACK HACK
 * I'd like to have a real dbus-listener here using libdbus/whatever, but for now
 * I'm going to implement a lame little hack to use dbus-monitor, as time is tight!
 * HACK HACK HACK
 *
 */
unsigned char pnd_dbusnotify_rediscover_p ( pnd_dbusnotify_handle h );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
