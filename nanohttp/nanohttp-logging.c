/** @file nanohttp-logging.c Logging functions */
/******************************************************************
*  $Id: nanohttp-logging.c,v 1.3 2007/11/03 22:40:11 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2007 Heiko Ronsdorf
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA  02111-1307, USA.
*
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#include "nanohttp-logging.h"

static int _nanohttp_logtype = NANOHTTP_LOG_FOREGROUND;
static nanohttp_loglevel_t _nanohttp_log_loglevel = NANOHTTP_LOG_DEBUG;
static char *_nanohttp_log_logfile = NULL;

#if defined WIN32 && defined __MINGW32__
char *
VisualC_funcname(const char *file, int line)
{
  static char buffer[256];
  int i;
 
  for (i = strlen(file) - 1; i > 0 && file[i] != '\\'; i--)
    /* nothing */ ;
  sprintf(buffer, "%s:%d", (file[i] != '\\') ? file : (file + i + 1), line);
  return buffer;
}
#endif

nanohttp_loglevel_t
nanohttp_log_set_loglevel(nanohttp_loglevel_t loglevel)
{
  nanohttp_loglevel_t old;
 
  old  = _nanohttp_log_loglevel;
  _nanohttp_log_loglevel = loglevel;

  return old;
}

nanohttp_loglevel_t
nanohttp_log_get_loglevel(void)
{
  return _nanohttp_log_loglevel;
}

int
nanohttp_log_set_logtype(int type)
{
  int old;

  old = _nanohttp_logtype;
  _nanohttp_logtype = type;

  return old;
}

const char *
nanohttp_log_get_logfile(void)
{
  return _nanohttp_log_logfile;
}

void
nanohttp_log_set_logfile(const char *filename)
{
  if (_nanohttp_log_logfile)
    free(_nanohttp_log_logfile);

  if (filename)
    _nanohttp_log_logfile = strdup(filename);
}

void
_nanohttp_log_printf(nanohttp_loglevel_t level, const char *format, ...)
{
  const char *filename;
  va_list ap;

  if (level < _nanohttp_log_loglevel)
    return;

  va_start(ap, format);

  if (_nanohttp_logtype & NANOHTTP_LOG_FOREGROUND)
    vfprintf(stdout, format, ap);

#ifdef HAVE_SYSLOG_H
  if (_nanohttp_logtype & NANOHTTP_LOG_SYSLOG)
  {
    int syslog_level;

    switch (level)
    {
      case NANOHTTP_LOGLEVEL_VERBOSE:
      case NANOHTTP_LOGLEVEL_DEBUG:
        syslog_level = LOG_DEBUG;
	break;
      case NANOHTTP_LOGLEVEL_INFO:
	syslog_level = LOG_INFO;
	break;
      case NANOHTTP_LOGLEVEL_WARN:
	syslog_level = LOG_WARNING;
	break;
      case NANOHTTP_LOGLEVEL_ERROR:
	syslog_level = LOG_ERR;
        break;
      case NANOHTTP_LOGLEVEL_FATAL:
	syslog_level = LOG_CRIT;
	break;
    }
    vsyslog(syslog_level, format, ap);
  }
#endif

  if ((filename = nanohttp_log_get_logfile()))
  {
    FILE *fp;

    if (!(fp = fopen(filename, "a")))
      fp = fopen(filename, "w");

    if (fp)
    {
      vfprintf(fp, format, ap);
      fflush(fp);
      fclose(fp);
    }
  }

  va_end(ap);
}
