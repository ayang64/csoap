/******************************************************************
 *  $Id: csoapurl.c,v 1.1 2003/03/25 22:17:28 snowdrop Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
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
 * Email: ayaz@jprogrammet.net
 ******************************************************************/
#include "csoapurl.h"
#include "csoaplog.h"

#include <stdio.h>
#include <string.h>


static
HSOAPURL CreateUrlFromIndex(const char* urlstr, int iprotocol, int ihost, int iport)
{
  const char *FUNC = "CreateUrlFromIndex";
  int len, size;
  char tmp[8];
  HSOAPURL url;

  SoapTraceEnter(FUNC, "Entering");

  url = (HSOAPURL)malloc(sizeof(struct SOAPURL));

  url->m_protocol = (char*)malloc(sizeof(char)*iprotocol+1);
  strncpy(url->m_protocol, urlstr, iprotocol);

  size = ihost - iprotocol - 3;
  url->m_host = (char*)malloc(sizeof(char)*size);
  strncpy(url->m_host, &urlstr[iprotocol+3], size);

  if (iport > ihost)
  {
    size = iport - ihost;
    strncpy(tmp, &urlstr[ihost+1], size);
    url->m_port = atoi(tmp);
  } else {
    url->m_port = 80;
  }

  len = strlen(urlstr);
  if (len > iport )
  {
    size = len - iport;
    url->m_context = (char*)malloc(sizeof(char)*size);
    strncpy(url->m_context, &urlstr[iport], size);
  } else {
    url->m_context = NULL;
  }

  SoapTraceEnter(FUNC, "Leaving url = %p", url);
  return url;
};


/*-----------------------------------------------------------------
  FUNCTION: SoapUrlCreate
/-----------------------------------------------------------------*/

HSOAPURL SoapUrlCreate(const char* urlstr)
{
  const char *FUNC = "SoapUrlCreate";
  int iprotocol;
  int ihost;
  int iport;
  int len;
  HSOAPURL url = NULL;

  SoapTraceEnter(FUNC, "Entering. URL = %s", urlstr?urlstr:"null");
  
  iprotocol = 0;
  len = strlen(urlstr);
  printf("len = %d\n", len);
  /* find protocol */
  while (urlstr[iprotocol] != ':' && urlstr[iprotocol] != '\0')
  {
/*    printf("protocol: urlstr[%d] = %c\n",iprotocol, urlstr[iprotocol] );*/
    iprotocol++;
  }

  if (iprotocol == 0) {
    SoapTraceLeaveWithError(FUNC, "no protocol");
    return NULL;
  }

  if (iprotocol + 3 >= len) {
    SoapTraceLeaveWithError(FUNC, "no host");
    return NULL;
  }

  if ( urlstr[iprotocol] != ':' 
    && urlstr[iprotocol+1] != '/' 
    && urlstr[iprotocol+2] != '/')
  {
    SoapTraceLeaveWithError(FUNC, "no protocol");
    return NULL;
  }

  /* find host */
  ihost = iprotocol + 3;
  while (urlstr[ihost] != ':'
    && urlstr[ihost] != '/'
    && urlstr[ihost] != '\0')
  {
/*    printf("host: urlstr[%d] = %c\n",ihost, urlstr[ihost] );*/
    ihost++;
  }

  if (ihost == iprotocol + 1) {
    SoapTraceLeaveWithError(FUNC, "no host");
    return NULL;
  }

  /* find port */
  iport = ihost;
  if (ihost + 1 < len) {
    if (urlstr[ihost] == ':') {
      while (urlstr[iport] != '/' && urlstr[iport] != '\0')  {
/*     printf("port: urlstr[%d] = %c\n", iport, urlstr[iport] );*/
       iport++;
      }
    }
  }

/*
  printf("iprotocol = %d\nihost = %d\niport = %d\nlen = %d\n",
      iprotocol, ihost, iport, len);
*/
  url = CreateUrlFromIndex(urlstr, iprotocol, ihost, iport);
  SoapTraceLeave(FUNC, "Leaving url (%p)", url);
  return url;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapUrlFree
/-----------------------------------------------------------------*/

void SoapUrlFree(HSOAPURL url)
{
  if (url != NULL) {
    if (url->m_protocol) free(url->m_protocol);
    if (url->m_host) free(url->m_host);
    if (url->m_context) free(url->m_context);

    free(url);
  }
}


/*-----------------------------------------------------------------
  FUNCTION: SoapUrlDump
/-----------------------------------------------------------------*/

void SoapUrlDump(HSOAPURL url)
{
  if (url == NULL) {
    printf("(null)\n");
    return ;
  }

  printf("PROTOCOL : %s\n", url->m_protocol?url->m_protocol:"(null)");
  printf("    HOST : %s\n", url->m_host?url->m_host:"(null)");
  printf("    PORT : %d\n", url->m_port);
  printf(" CONTEXT : %s\n", url->m_context?url->m_context:"(null)");

}
