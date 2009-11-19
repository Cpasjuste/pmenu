/***************************************************************************
                          cpu_load.c  -  description
                             -------------------
    begin                : mer jun 2 2004
    copyright            : (C) 2004 by Nicolas Dos Santos
    email                : nicolas.dos-santos@wanadoo.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cpuLoad.h"

static int initialized_ = NO;

/**
 * Get the current time (works just as "time", just that we use the
 * unit of time that the cron-jobs use (and is 64 bit)).
 *
 * @param setme will set the current time if non-null
 * @return the current time
 **/
cron_t cronTime()
{
	cron_t res;
	struct timeval tv;

	gettimeofday(&tv, NULL);//&tz);

	res = (((cron_t)tv.tv_sec) * CRON_UNIT_TO_SECONDS) +
		(tv.tv_usec / MICROSEC_TO_CRON_UNIT);

	return res;
}

static FILE * proc_stat = NULL;


/**
 * The following method is called in order to initialize the status calls
 * routines.  After that it is safe to call each of the status calls separately
 **/
void initStatusCalls()
{
	initialized_ = YES;

	proc_stat = fopen("/proc/stat", "r");

	if (proc_stat == NULL)
	{
		printf("ERROR: Could not open /proc/stat \n");
		exit(0);
	}

	cpuUsage();
}

/**
 * Shutdown the status calls module.
 **/
void doneStatusCalls()
{

	if (proc_stat != NULL)
		fclose(proc_stat);

	initialized_ = NO;
}

/**
 * The following routine returns a number between 0-1 (can be larger than 1
 * if the load is > 1) which indicates the percentage CPU usage.
 *
 * Before its first invocation the method initStatusCalls() must be called.
 * If there is an error the method returns -1
 **/
float cpuUsage()
{
  static cron_t lastcputime = 0;
  static float lastcpuresult = -1;
  cron_t now, elapsedtime;

  if (initialized_ == NO)
    return -1;
  now = cronTime();

  elapsedtime = now - lastcputime;
  if ( (elapsedtime < NB_SEC * cronSECONDS) &&
       (lastcpuresult != -1) ) {
    return lastcpuresult;
  }

  lastcputime = now;
  /* under linux, first try %idle/usage using /proc/stat;
     if that does not work, disable /proc/stat for the future
     by closing the file and use the next-best method. */

  if (proc_stat != NULL) {

    static int last_cpu_results[4] = { 0, 0, 0, 0 };
    float ret = -1;
    char line[128];
    int user_read, system_read, nice_read, idle_read;
    int user, system, nice, idle;
    float usage_time=0.0, total_time=1.0;

    /* Get the first line with the data */
    rewind(proc_stat);
    fflush(proc_stat);
    if (fgets(line, 128, proc_stat)==NULL) {
      printf("ERROR: reading /proc/stat file");
      fclose(proc_stat);
      printf("ERROR: could not read /proc/stat file\n");
      proc_stat = NULL;
    } else {
      if (sscanf(line, "%*s %i %i %i %i",
                 &user_read, &system_read, &nice_read,
                 &idle_read) != 4) {
        fclose(proc_stat);
        printf("ERROR: decoding /proc/stat file\n");
        proc_stat = NULL; /* don't try again */
      } else {

	/* Store the current usage*/
        user = user_read - last_cpu_results[0];
        system = system_read - last_cpu_results[1];
        nice = nice_read - last_cpu_results[2];
        idle = idle_read - last_cpu_results[3];
        /* Calculate the % usage */
        if ((user + system + nice + idle) > 0) {
          usage_time = user + system + nice;
          total_time = usage_time + idle;
        }
        if (total_time == 0)
          total_time = 1; /* avoid fpe */
        ret = (1 * usage_time) / total_time;

        /* Store the values for the next calculation*/
        last_cpu_results[0] = user_read;
        last_cpu_results[1] = system_read;
        last_cpu_results[2] = nice_read;
        last_cpu_results[3] = idle_read;
        lastcpuresult = ret;

        return lastcpuresult;
      }
    }
  }

   /* loadaverage not defined and no platform
      specific alternative defined
      => default: error
   */
   lastcpuresult = -1;
   return -1;
 }

/**
  * Get the load of the CPU relative to what is allowed.
  * @return the CPU load as a percentage of allowed
  *        (1 is equivalent to full load)
  **/
float getCPULoad() {
   static float lastRet = 0.0;
   float ret;

   if (initialized_ == NO)
     return -1;

   ret = (1 * cpuUsage()) / 1;

   /* for CPU, we don't do the 'fast increase' since CPU is much
      more jitterish to begin with */
   lastRet = (ret + 7 * lastRet)/8;

   return lastRet;
}

