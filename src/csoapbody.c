/******************************************************************
 *  $Id: csoapbody.c,v 1.1 2003/03/25 22:17:18 snowdrop Exp $
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
#include "csoapbody.h"
#include "csoaplog.h"


/*-----------------------------------------------------------------
  FUNCTION: SoapBodyGetMethod
/-----------------------------------------------------------------*/

HSOAPMETHOD SoapBodyGetMethod(HSOAPBODY body, const char* method)
{
  HSOAPXML node;
  const char* FUNC = "SoapBodyGetMethod";
  SoapTraceEnter(FUNC,"Method = %s", method);

  if (body == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid body (null)");
    return NULL;
  }

  SoapLog(LOG_DEBUG, FUNC, "Searching method '%s'", method);

  node = SoapXmlFindNode((HSOAPXML)body, method);
  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "Can not find method");
    return NULL;
  }
/*
  cur = ((xmlNodePtr)body)->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlStrcmp(cur->name, (const xmlChar*)method)) {
      SoapTraceLeave(FUNC, "Found method (%p)", cur);
      return (HSOAPMETHOD)cur;
    }
    cur = cur->next;
  }
*/
  SoapTraceLeave(FUNC, "Method (%p)", node);
  return node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapBodyGetFault
/-----------------------------------------------------------------*/

HSOAPFAULT SoapBodyGetFault(HSOAPBODY body)
{
  HSOAPXML node;
  const char* FUNC = "SoapBodyGetFault";
  SoapTraceEnter(FUNC,"Body (%p)", body);

  if (body == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid body (null)");
    return NULL;
  }

/*
  cur = ((xmlNodePtr)body)->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlStrcmp(cur->name, (const xmlChar*)"Fault")) {
      SoapTraceLeave(FUNC, "Found fault (%p)", cur);
      return (HSOAPFAULT)cur;
    }
    cur = cur->next;
  }
*/

  node = SoapXmlFindNode((HSOAPXML)body, "Fault");
  SoapTraceLeave(FUNC, "Fault (%p)", node);
  return node;
}
