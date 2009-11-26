/***************************************************************************
                          cpu_load.h  -  description
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

#ifndef UTILS_CPU_H
#define UTILS_CPU_H

#include <stdio.h>
#include <sys/time.h>  // for gettimeoftheday
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define NOMFICH_CPUINFO "/proc/cpuinfo"
#define _int16 short
#define _int32 int
#define _int64 long long int
#define _float32 float
#define _float4 double

#  define NB_SEC 1

/** For releases, this value should always be 1 */
#define SPEED_UP 1

/** number of cron units (ms) in a second **/
#define CRON_UNIT_TO_SECONDS (1000 / SPEED_UP)

/** number of ns [usec] in a cron-unit (1000000) **/
#define NANOSEC_TO_CRON_UNIT (1000 * 1000 * SPEED_UP)

/** number of us [usec] in a cron-unit (1000) **/
#define MICROSEC_TO_CRON_UNIT (1000 * SPEED_UP)

/**
 * @brief Module to run background jobs.
 **/

/* use these constants to specify timeings */
#define cronMILLIS 1
#define cronSECONDS (1000 * cronMILLIS)
#define cronMINUTES (60 * cronSECONDS)
#define cronHOURS (60 * cronMINUTES)
#define cronDAYS (24 * cronHOURS)
#define cronWEEKS (7 * cronDAYS)
#define cronMONTHS (30 * cronDAYS)
#define cronYEARS (365 * cronDAYS)

#define OK 1
#define SYSERR -1

#define YES 1
#define NO 0

/**
 * Time for absolute times used by cronTime.
 **/
typedef unsigned _int64 cron_t;

cron_t cronTime();

void initStatusCalls();

void doneStatusCalls();

float cpuUsage();

float getCPULoad();


/* ifndef CPULOAD_H */
#endif
/* end of CPULOAD.h */
