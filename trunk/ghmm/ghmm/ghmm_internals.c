/*******************************************************************************
*
*       This file is part of the General Hidden Markov Model Library,
*       GHMM version __VERSION__, see http://ghmm.org
*
*       Filename: ghmm/ghmm/logging.c
*       Authors:  Janne Grunau
*
*       Copyright (C) 1998-2004 Alexander Schliep 
*       Copyright (C) 1998-2001 ZAIK/ZPR, Universitaet zu Koeln
*	Copyright (C) 2002-2004 Max-Planck-Institut fuer Molekulare Genetik, 
*                               Berlin
*                                   
*       Contact: schliep@ghmm.org             
*
*       This library is free software; you can redistribute it and/or
*       modify it under the terms of the GNU Library General Public
*       License as published by the Free Software Foundation; either
*       version 2 of the License, or (at your option) any later version.
*
*       This library is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*       Library General Public License for more details.
*
*       You should have received a copy of the GNU Library General Public
*       License along with this library; if not, write to the Free
*       Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*       This file is version $Revision$ 
*                       from $Date$
*             last change by $Author$.
*
*******************************************************************************/


#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ghmm.h"
#include "ghmm_internals.h"
#include "mprintf.h"


static char * qmessage;

static int maxlevel = 3;

static void (* logfunc)(int level, const char * message, void * clientdata);

static void * logfunc_data;

static void process_external_log(int level, const char* proc, const char* error_str) {

  char* message;

  int len = strlen(proc) + strlen(error_str) + 1;
  message = malloc(sizeof(char)*(len+1));
  if (!message)
    return;
  message = strncpy(message, proc, len);
  message = strncat(message, error_str, len);
  
  logfunc(level, message, logfunc_data);
}

static void ighmm_log_out(int level, const char* proc, const char* message) {

  /* if defined use external logging function */
  if (logfunc) {
    process_external_log(level, proc, message);
  }
  /* otherwise simple logging to stderr */
  else 
    if (level < maxlevel) {
      switch (level) {
      case LDEBUG:
	fputs("DEBUG: ", stderr);
	break;
      case LINFO:
	fputs("INFO: ", stderr);
	break;
      case LWARN:
	fputs("WARNING: ", stderr);
	break;
      case LERROR:
	fputs("ERROR: ", stderr);
	break;
      case LCRITIC:
	fputs("CRITICAL: ", stderr);
	break;
      default:
	break;
      }
      fputs(proc, stderr);
      fputs(message, stderr);
      fputc('\n', stderr);
    }

  free(message);
}


void GHMM_LOG_PRINTF(int level, const char* proc, const char* error_str, ...) {
  char* message;
  va_list args;

  /* process queued message if any */
  if (qmessage) {
    ighmm_log_out(level, proc, qmessage);
    qmessage = NULL;
  }
  if (error_str) {
    va_start(args, error_str);
    message = ighmm_mprintf(NULL, 0, error_str, args);
    ighmm_log_out(level, proc, message);
    va_end(args);
  }
}

void ighmm_queue_mes(char * text) {
#define CUR_PROC "ighmm_queue_mes"
  /* let no message get lost */
  if (!qmessage)
    qmessage = text;
  else {
    ighmm_log_out(LCRITIC, LOC ": unable to queue message since it exists an unprocessed message! ", text);
    exit(3);
  }
#undef CUR_PROC
}

void ghmm_set_logfunc(void (* fptr)(int, const char *, void *), void * clientdata) {

  logfunc = fptr;
  logfunc_data = clientdata;

}

void ghmm_set_loglevel(int level) {

  maxlevel = level;

}
