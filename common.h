#ifndef COMMON_H
#define COMMON_H

#define HEXTOR(c) ((c>>24)&0xFF)
#define HEXTOG(c) ((c>>16)&0xFF)
#define HEXTOB(c) ((c>>8)&0xFF)
#define HEXTOA(c) ((c)&0xFF)

#define RESET		0
#define BRIGHT 		1
#define DIM	        2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED         1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

#ifdef I386
#define PMENU_PATH "./"
#else
#define PMENU_PATH "/etc/pandora/pmenu"
#endif

void textcolor( int attr, int fg );
int debug_func;
int debug_func_count;

#define DPRINTF printf

#define debug_startf( fmt, ... ) \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    textcolor( BRIGHT, GREEN );	\
	DPRINTF( "FUNC_START: (%s():%s:%d): ", __func__, __FILE__, __LINE__ );\
	DPRINTF( fmt, __VA_ARGS__ );\
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "{\n"); \
	textcolor( RESET, RESET );	\
	debug_func++;

#define debug_start( ) \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    textcolor( BRIGHT, GREEN );	\
	DPRINTF( "FUNC_START: %s():%s:%d\n", __func__, __FILE__, __LINE__ );\
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "{\n"); \
	textcolor( RESET, RESET ); \
	debug_func++;

#define debug_infof( fmt, ... ) \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "INFO: ");\
	DPRINTF( fmt, __VA_ARGS__ );\
	DPRINTF( "\n" );

#define debug_info( str ) \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "INFO: ");\
	DPRINTF( str );\
	DPRINTF( " ( %s:%d )", __FILE__, __LINE__ );\
	DPRINTF( "\n" );

#define debug_errorf( fmt, ... ) \
    textcolor( BRIGHT, RED ); \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    textcolor( BRIGHT, RED ); \
	DPRINTF( "ERROR: ");\
	DPRINTF( fmt, __VA_ARGS__ );\
	DPRINTF( " ( %s:%d )", __FILE__, __LINE__ );\
	textcolor( RESET, RESET ); \
	DPRINTF( "\n" );

#define debug_error( str ) \
    textcolor( BRIGHT, RED );	\
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    textcolor( BRIGHT, RED );	\
	DPRINTF( "ERROR: ");\
	DPRINTF( str );\
	DPRINTF( " ( %s:%d )", __FILE__, __LINE__ );\
	textcolor( RESET, RESET ); \
	DPRINTF( "\n" );

#define debug_endf( fmt, ... ) \
    debug_func--; \
    textcolor( BRIGHT, GREEN );	\
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    DPRINTF( "}\n"); \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "FUNC_END: %s():%s:%d: ", __func__, __FILE__, __LINE__ );\
	DPRINTF( fmt, __VA_ARGS__ );\
    DPRINTF( "\n"); \
	textcolor( RESET, RESET );	\

#define debug_end( ) \
    debug_func--; \
    textcolor( BRIGHT, GREEN );	\
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
    DPRINTF( "}\n"); \
    for ( debug_func_count = 0; debug_func_count < debug_func; debug_func_count++ ) \
        DPRINTF( "   "); \
	DPRINTF( "FUNC_END: %s():%s:%d\n", __func__, __FILE__, __LINE__ );\
    DPRINTF( "\n"); \
	textcolor( RESET, RESET );

#endif

