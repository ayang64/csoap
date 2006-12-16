/******************************************************************
*  $Id: nanohttp-url.c,v 1.3 2006/12/16 16:09:45 m0gg Exp $
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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-url.h"
      
#define HTTP_DEFAULT_PORT		80
#define HTTPS_DEFAULT_PORT		443

static void
_hurl_dump(const struct hurl_t *url)
{
  if (!url)
  {
    log_error1("parameter url is NULL");
    return;
  }

  log_verbose2("PROTOCOL: %d", url->protocol);
  log_verbose2("    HOST: \"%s\"", url->host);
  log_verbose2("    PORT: %d", url->port);
  log_verbose2(" CONTEXT: \"%s\"", url->context);

  return;
}

herror_t
hurl_parse(struct hurl_t *url, const char *urlstr)
{
  int iprotocol;
  int ihost;
  int iport;
  int len;
  int size;
  char tmp[8];
  char protocol[1024];
  struct servent *entry;

  iprotocol = 0;
  len = strlen(urlstr);

  /* find protocol */
  while (urlstr[iprotocol] != ':' && urlstr[iprotocol] != '\0')
  {
    iprotocol++;
  }

  if (iprotocol == 0)
  {
    log_error1("no protocol");
    return herror_new("hurl_parse", URL_ERROR_NO_PROTOCOL, "No protocol");
  }
  if (iprotocol + 3 >= len)
  {
    log_error1("no host");
    return herror_new("hurl_parse", URL_ERROR_NO_HOST, "No host");
  }
  if (urlstr[iprotocol] != ':'
      && urlstr[iprotocol + 1] != '/' && urlstr[iprotocol + 2] != '/')
  {
    log_error1("no protocol");
    return herror_new("hurl_parse", URL_ERROR_NO_PROTOCOL, "No protocol");
  }

  /* find host */
  ihost = iprotocol + 3;
  while (urlstr[ihost] != ':'
         && urlstr[ihost] != '/' && urlstr[ihost] != '\0')
  {
    ihost++;
  }

  if (ihost == iprotocol + 1)
  {
    log_error1("no host");
    return herror_new("hurl_parse", URL_ERROR_NO_HOST, "No host");
  }

  /* find port */
  iport = ihost;
  if (ihost + 1 < len)
  {
    if (urlstr[ihost] == ':')
    {
      while (urlstr[iport] != '/' && urlstr[iport] != '\0')
      {
        iport++;
      }
    }
  }

  /* find protocol */
  strncpy(protocol, urlstr, iprotocol);
  protocol[iprotocol] = '\0';
  if (!strncasecmp(protocol, "http", 5))
    url->protocol = PROTOCOL_HTTP;
  else if (!strncasecmp(protocol, "https", 6))
    url->protocol = PROTOCOL_HTTPS;
  else
    return herror_new("hurl_parse", URL_ERROR_UNKNOWN_PROTOCOL, "Unknown protocol \"%s\"", protocol);

  /* find right port */
  if (!(entry = getservbyname(protocol, "tcp")))
  {
    log_warn2("getservbyname(\"%s\", \"tcp\") returned NULL, please edit services database", protocol);

    switch (url->protocol)
    {
    case PROTOCOL_HTTP:
      url->port = HTTP_DEFAULT_PORT;
      break;
    case PROTOCOL_HTTPS:
      url->port = HTTPS_DEFAULT_PORT;
      break;
    }
  }
  else
  {
    url->port = ntohs(entry->s_port);
  }

  size = ihost - iprotocol - 3;
  if (!(url->host = (char *)malloc(size + 1)))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return herror_new("hurl_parse", URL_ERROR, "malloc failed (%s)", strerror(errno));
  }
  strncpy(url->host, &urlstr[iprotocol + 3], size);
  url->host[size] = '\0';

  if (iport > ihost)
  {
    size = iport - ihost;
    strncpy(tmp, &urlstr[ihost + 1], size);
    url->port = atoi(tmp);
  }

  /* find path */
  len = strlen(urlstr);
  if (len > iport)
  {
    size = len - iport;
    if (!(url->context = (char *)malloc(size + 1)))
    {
      log_error2("malloc failed (%s)", strerror(errno));
      return herror_new("hurl_parse", URL_ERROR, "malloc failed (%s)", strerror(errno));
    }
    strncpy(url->context, &urlstr[iport], size);
    url->context[size] = '\0';
  }
  else
  {
    url->context = strdup("");
  }

  _hurl_dump(url);

  return H_OK;
}

void
hurl_free(struct hurl_t *url)
{
  if (url)
  {
    if (url->host)
      free(url->host);

    if (url->context)
      free(url->context);

    free(url);
  }

  return;
}
