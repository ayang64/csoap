/******************************************************************
*  $Id: nanohttp-url.c,v 1.1 2006/12/08 21:21:41 m0gg Exp $
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

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-url.h"
      
/* TODO (#1#): find proper ports */
#define URL_DEFAULT_PORT_HTTP 80
#define URL_DEFAULT_PORT_HTTPS 81
#define URL_DEFAULT_PORT_FTP 120

static void
_hurl_dump(const struct hurl_t * url)
{

  if (url == NULL)
  {
    log_error1("url is NULL!");
    return;
  }
  log_verbose2("PROTOCOL : %d", url->protocol);
  log_verbose2("    HOST : %s", url->host);
  log_verbose2("    PORT : %d", url->port);
  log_verbose2(" CONTEXT : %s", url->context);
}

herror_t
hurl_parse(struct hurl_t * url, const char *urlstr)
{
  int iprotocol;
  int ihost;
  int iport;
  int len;
  int size;
  char tmp[8];
  char protocol[1024];

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
  if (strcasecmp(protocol, "http"))
    url->protocol = PROTOCOL_HTTP;
  else if (strcasecmp(protocol, "https"))
    url->protocol = PROTOCOL_HTTPS;
  else if (strcasecmp(protocol, "ftp"))
    url->protocol = PROTOCOL_FTP;
  else
    return herror_new("hurl_parse", URL_ERROR_UNKNOWN_PROTOCOL, "Unknown protocol '%s'", protocol);

  /* TODO (#1#): add max of size and URL_MAX_HOST_SIZE */
  size = ihost - iprotocol - 3;
  strncpy(url->host, &urlstr[iprotocol + 3], size);
  url->host[size] = '\0';

  if (iport > ihost)
  {
    size = iport - ihost;
    strncpy(tmp, &urlstr[ihost + 1], size);
    url->port = atoi(tmp);
  }
  else
  {
    switch (url->protocol)
    {
    case PROTOCOL_HTTP:
      url->port = URL_DEFAULT_PORT_HTTP;
      break;
    case PROTOCOL_HTTPS:
      url->port = URL_DEFAULT_PORT_HTTPS;
      break;
    case PROTOCOL_FTP:
      url->port = URL_DEFAULT_PORT_FTP;
      break;
    }
  }

  len = strlen(urlstr);
  if (len > iport)
  {
    /* TODO (#1#): find max of size and URL_MAX_CONTEXT_SIZE */
    size = len - iport;
    strncpy(url->context, &urlstr[iport], size);
    url->context[size] = '\0';
  }
  else
  {
    url->context[0] = '\0';
  }

  _hurl_dump(url);

  return H_OK;
}

void
hurl_free(struct hurl_t *url)
{
  if (!url)
    return;

  free(url);

  return;
}
