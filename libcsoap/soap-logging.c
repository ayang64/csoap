/******************************************************************
*  $Id: soap-logging.c,v 1.1 2007/11/03 22:40:09 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
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
* Email: ayaz@jprogrammer.net
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
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

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#include "soap-logging.h"

static int _soap_logtype = SOAP_LOG_FOREGROUND;
static soap_loglevel_t _soap_loglevel = SOAP_LOG_DEBUG;
static char *_soap_log_logfile = NULL;

soap_loglevel_t
soap_log_set_loglevel(soap_loglevel_t level)
{
  soap_loglevel_t old = _soap_loglevel;
  _soap_loglevel = level;
  return old;
}

soap_loglevel_t
soap_log_get_loglevel(void)
{
  return _soap_loglevel;
}

int
soap_log_set_logtype(int type)
{
  int old;

  old = _soap_logtype;
  _soap_logtype = type;

  return old;
}

const char *
soap_log_get_logfile(void)
{
  return _soap_log_logfile;
}

void
soap_log_set_logfile(const char *filename)
{
  if (_soap_log_logfile)
    free(_soap_log_logfile);

  if (filename)
    _soap_log_logfile = strdup(filename);
}

void
_soap_log_printf(soap_loglevel_t level, const char *format, ...)
{
  const char *filename;
  va_list ap;

  if (level < _soap_loglevel)
    return;

  va_start(ap, format);

  if (_soap_logtype & SOAP_LOG_FOREGROUND)
    vfprintf(stdout, format, ap);

#ifdef HAVE_SYSLOG_H
  if (_nanohttp_logtype & SOAP_LOG_SYSLOG)
  {
    int syslog_level;

    switch (level)
    {
      case SOAP_LOGLEVEL_VERBOSE:
      case SOAP_LOGLEVEL_DEBUG:
        syslog_level = LOG_DEBUG;
	break;
      case SOAP_LOGLEVEL_INFO:
        syslog_level = LOG_INFO;
	break;
      case SOAP_LOGLEVEL_WARN:
        syslog_level = LOG_WARNING;
	break;
      case SOAP_LOGLEVEL_ERROR:
        syslog_level = LOG_ERR;
	break;
      case SOAP_LOGLEVEL_FATAL:
        syslog_level = LOG_CRIT;
	break;
    }
    vsyslog(syslog_level, format, ap);
  }
#endif

  if ((filename = soap_log_get_logfile()))
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

