#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/statvfs.h>

#include "common.h"
#include "config_pmenu.h"

void textcolor( int attr, int fg )
{
    char command[13];

	/* Command is the control command to the terminal */
	sprintf( command, "%c[%d;%dm", 0x1B, attr, fg + 30 );
	printf( "%s", command );
}

void error(char *msg)
{
    printf ("%s (line %d of %s)\n", msg, (__LINE__ -1), __FILE__);
}

int copy(char *src, char *dst)
{
    FILE *in = fopen(src, "r");
    FILE *out = fopen(dst, "w");

    int len;

    char buffer[1024];

    while((len = fread(buffer,1,sizeof(buffer),in)) > 0)
    {
        fwrite(buffer,1,len,out);
    }

    fclose(in);
    fclose(out);

    return 1;
}

char* get_time_string(int mode24)
{
  static char time_str[12] = {0};   /* Stroes the time as a string */
  struct tm *now = NULL;
  int hour = 0;
  time_t time_value = 0;

  time_value = time(NULL);          /* Get time value              */
  now = localtime(&time_value);     /* Get time and date structure */
  hour = now->tm_hour;              /* Save the hour value         */
  if(!mode24 && hour>12)            /* Adjust if 12 hour format    */
    hour -= 12;

    sprintf(time_str, "%02d:%02d",hour, now->tm_min);

/*
    if(!mode24)
        if(now->tm_hour>24){
       printf("PM\n");
    }else{
       printf("AM\n");
     }
*/
    return time_str;
}

char *disk_space(char *path)
{
    static char space_str[12] = {0};
    struct statvfs fiData;

    if((statvfs(path,&fiData)) < 0 )
    {
        sprintf( space_str, "No info" );
        return space_str;
    }
    else
    {
        float totalspace=((float) fiData.f_bsize)*((float) fiData.f_blocks);
        float freespace=((float) fiData.f_bsize)*((float) fiData.f_bavail);

        sprintf( space_str, "%.0fMB/%.0fMB", (totalspace/(1024.0*1024.0)) - (freespace/(1024.0*1024.0)), totalspace/(1024.0*1024.0) );

        return space_str;
/*
        printf("Free space:%.2fMB, %.2fGB,\n",freespace/(1024.0*1024.0),freespace/(1024.0*1024.0*1024.0));
        printf("Total space: %.2fMB, %.2fGB,\n", totalspace/(1024.0*1024.0),totalspace/(1024.0*1024.0*1024.0));
*/
    }

    sprintf( space_str, "No info" );
    return space_str;
}

void set_cpu( int mhz )
{
    if ( ( mhz >= 30 ) && ( mhz <= 800 ) )
    {

        char command[256];
        memset( command, 0, 256 );

        sprintf( command, "echo %i > /proc/pandora/cpu_mhz_max", mhz );

        int ret;
        ret = system( command );

       // pnd_device_set_clock( mhz );
    }
}

const char *mystristr(const char *haystack, const char *needle)
{
   if ( !*needle )
   {
      return haystack;
   }
   for ( ; *haystack; ++haystack )
   {
      if ( toupper(*haystack) == toupper(*needle) )
      {
         /*
          * Matched starting char -- loop through remaining chars.
          */
         const char *h, *n;
         for ( h = haystack, n = needle; *h && *n; ++h, ++n )
         {
            if ( toupper(*h) != toupper(*n) )
            {
               break;
            }
         }
         if ( !*n ) /* matched all of 'needle' to null termination */
         {
            return haystack; /* return the start of the match */
         }
      }
   }
   return 0;
}

int is_avi( char *filename )
{
    debug_start();
    debug_infof( "%s", filename );
    const char *avi = mystristr( filename, ".avi" );

    if ( avi )
    {
        debug_end();
        return 1;
    }

    debug_end();
    return 0;
}

int is_img( char *filename )
{
    debug_start();
    debug_infof( "%s", filename );
    const char *png = mystristr( filename, ".png" );
    const char *bmp = mystristr( filename, ".bmp" );

    if ( png || bmp )
    {
        debug_end();
        return 1;
    }

    debug_end();
    return 0;
}



