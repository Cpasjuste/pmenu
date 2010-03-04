
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
// after a write, do a flush; may only apply to streams, but will attempt to apply to all
void pnd_log_set_flush ( unsigned char x );

// set a 'filter level'; any log message of higher-or-equal level than current filter-level will be emitted. Thus, to remove filters
// just set to level 0. Returns existing setting.
unsigned char pnd_log_set_filter ( unsigned char newlevel );                // ex: app-specific enum/#defines for your levels
unsigned char pnd_log_get_filter ( void );

// how many targets can be opened, entirely? this is a compile time limit, for sanity.
unsigned char pnd_log_max_targets ( void );

// tell buried lib functions to do some logging here; ie: since not all API's have a 'options' type flag where we could
// add a hint to begin logging, they can just ask the logger if they need to do it
#define PND_LOG_DEFAULT 0 // level 0 is by default the 'debug' level, with anything higher non-debug; only used for buried logging
void pnd_log_set_buried_logging ( unsigned char yesno ); // set 1 for 'yes', 0 for 'no'
unsigned char pnd_log_do_buried_logging ( void ); // return 1 for 'yes'

#ifdef __cplusplus
} /* "C" */
#endif

#endif
