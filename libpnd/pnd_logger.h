
#ifndef h_pnd_logger_h
#define h_pnd_logger_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* rudimentary logger; note that this is meant to be a no-brainer to use, so no setup is needed.
 */
#define PND_LOG_FORCE (0xFF) /* use this log level to require it be emitted, no skipping */

// defaults will have no filtering, so any message will be emitted to all targets
// default target is nothing, however, so logger is silent unless activated
unsigned char pnd_log ( unsigned char level, char *format, ... );           // returns true if emitted; \n is implied!
unsigned char pnd_log_to_stdout ( void );                                   // same as pnd_log_to_stream ( stdout );
unsigned char pnd_log_to_stderr ( void );                                   // same as pnd_log_to_stream ( stderr );

/* the below is all optional, for when you need more control
 */

// logging is additive; you can log to multiple targets at once. Returns 'true' if accepted, false if could not set up.
void pnd_log_to_nil ( void );                                               // stop logging to anywhere; does not close streams/etc
unsigned char pnd_log_to_stream ( FILE * );                                 // 'stdout', 'stderr', or your own FILE* are good values
unsigned char pnd_log_to_syslog ( char *facility );                         // NYI
typedef void (*pnd_log_callback_f)( char *text, void *userdata );
unsigned char pnd_log_to_callback ( pnd_log_callback_f f, void *userdata ); // NYI

// pass NULL to free any pre-text, otherwise it'll be kept. Passed in string is duplicated, so you may free yours if you like.
void pnd_log_set_pretext ( char * );                                        // example: your app-name, or app+function-names, say.

// set a 'filter level'; any log message of higher-or-equal level than current filter-level will be emitted. Thus, to remove filters
// just set to level 0. Returns existing setting.
unsigned char pnd_log_set_filter ( unsigned char newlevel );                // ex: app-specific enum/#defines for your levels
unsigned char pnd_log_get_filter ( void );

// how many targets can be opened, entirely? this is a compile time limit, for sanity.
unsigned char pnd_log_max_targets ( void );

#ifdef __cplusplus
} /* "C" */
#endif

#endif
