/******************************************************************
 *  $Id: csoaphttp.c,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#include "libcsoap/csoaphttp.h"
#include "libcsoap/csoaplog.h"
#include "libcsoap/csoapxml.h"

/*-----------------------------------------------------------------
  FUNCTION: SoapHttpCreateEnv
/-----------------------------------------------------------------*/

static
HSOAPENV SoapHttpCreateEnv(char* httpResponse)
{
  const char *FUNC = "SoapHttpCreateEnv";
  int count = 0;
  int newline = 0;
  xmlDocPtr env;
  xmlNodePtr node;
  int len;
  char *tmp;

  SoapTraceEnter(FUNC, "Entering response =\n");
  while (*(httpResponse+count) != '\0') {
    if (httpResponse[count]=='\r') {
      count++;
      continue;
    }
    if (httpResponse[count]=='\n') {
      if (newline) break;
      else newline = 1;
    } else newline = 0;

    count++;
  }

  len = strlen(httpResponse);
  SoapLog(LOG_DEBUG, FUNC, "count = %d\n", count);
  SoapLog(LOG_DEBUG, FUNC, "response length = %d\n", len);

  len = strlen(&httpResponse[count]);
  SoapLog(LOG_DEBUG, FUNC, "xml response length = %d\n", len);

  while (httpResponse[count] == '\n'
	 || httpResponse[count] == '\r') {
    count++;
  }
   
  SoapLog(LOG_DEBUG, FUNC, "count = %d\n", count);

  SoapLog(LOG_DEBUG, FUNC, "casting\n");
  tmp = (char*)(&httpResponse[count]);
  SoapLog(LOG_DEBUG, FUNC, "casting ok\n");

  /*  SoapLog(LOG_DEBUG, FUNC, "xml = %s\n",tmp?tmp:"null"); 
   */
  SoapLog(LOG_DEBUG, FUNC, "Starting parsing\n");
  env = xmlParseMemory((const char*)tmp, len);
  if (env == NULL) {
    SoapLog(LOG_ERROR, FUNC,
	    "Can not create xml document!");
    return NULL;
  }

  node = xmlDocGetRootElement(env);
  if (node == NULL) {
    SoapLog(LOG_ERROR, FUNC,
	    "xml document is empty!");
    return NULL;
  }

  SoapLog(LOG_DEBUG, FUNC,
	  "Envelope created successfully");

  SoapTraceLeave(FUNC, "env (%p)", node);
  return (HSOAPENV)node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapHttpSend
/-----------------------------------------------------------------*/

CSOAP_STATUS SoapHttpSend(HSOAPURL url, HSOAPENV env, HSOAPENV* res)
{
  const char *FUNC = "SoapHttpSend";
  HSOAPSOCKET http;
  int len;
  ssize_t size;
  char *tmp;
  char *response;
  char tmp2[1054];
  CSOAP_STATUS status;
  xmlBufferPtr buffer;
  xmlDocPtr doc;

  SoapTraceEnter(FUNC, "env (%p)", env);

  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, ((xmlNodePtr)env)->doc, (xmlNodePtr)env, 1 ,1);
  len = strlen((const char*)xmlBufferContent(buffer));

  tmp = (char*)malloc(1054 + len);
  if (tmp == NULL)
    return ERROR_NO_MEM;

  status = SoapSocketCreate(url->m_host, url->m_port, &http);
  if (status != CSOAP_OK) return status;

  /*SoapSocketGetResponse(http);*/
  sprintf(tmp2, "POST %s HTTP/1.0\n", url->m_context?url->m_context:"/");
  sprintf(tmp2, "%sContent-Type: text/xml\n", tmp2);
  sprintf(tmp2, "%sContent-Length: %d\n\n", tmp2, len);

  sprintf(tmp, "%s%s", tmp2, (const char*)xmlBufferContent(buffer));

  SoapLog(LOG_DEBUG, FUNC, "sending request\n");
  status = SoapSocketSend(http, tmp, &size);
  if (status != CSOAP_OK) return status;

  status = SoapSocketRecv(http, &response, &size);
  SoapLog(LOG_DEBUG, FUNC, "response returned status = %d\n", status);
  if (status != CSOAP_OK) return status;
  
  *res = SoapHttpCreateEnv(response);
  free(tmp);
  free(response);
  xmlBufferFree(buffer);

  SoapTraceLeave(FUNC, "");
  return CSOAP_OK;
}

