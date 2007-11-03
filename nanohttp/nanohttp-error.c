/** @file nanohttp-error.c Error handling */
/******************************************************************
*  $Id: nanohttp-error.c,v 1.2 2007/11/03 22:40:11 m0gg Exp $
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"

typedef struct _herror_impl_t
{
  int errcode;
  char message[250];
  char *func;
} herror_impl_t;

herror_t
herror_new(const char *func, int errcode, const char *format, ...)
{
  va_list ap;
  herror_impl_t *impl;
 
  if (!(impl = (herror_impl_t *) malloc(sizeof(herror_impl_t))))
  {
    log_error("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  impl->errcode = errcode;
  impl->func = func ? strdup(func) : NULL;

  va_start(ap, format);
  vsprintf(impl->message, format, ap);
  va_end(ap);

  return (herror_t) impl;
}

int
herror_code(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return H_OK;

  return impl->errcode;
}

const char *
herror_func(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return "";

  return impl->func;
}

const char *
herror_message(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return "";

  return impl->message;
}

void
herror_release(herror_t err)
{
  herror_impl_t *impl = (herror_impl_t *) err;

  if (!err)
    return;

  if (impl->func)
    free(impl->func);

  free(impl);

  return;
}
