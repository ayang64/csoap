/******************************************************************
 *  $Id: csoapenv.c,v 1.2 2003/11/13 10:44:10 snowdrop Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#include "libcsoap/csoapenv.h"
#include "libcsoap/csoaplog.h"

static char *soap_env_ns = "http://schemas.xmlsoap.org/soap/envelope/";
static char *soap_env_enc = "http://schemas.xmlsoap.org/soap/encoding/";
static char *soap_xsi_ns = "http://www.w3.org/1999/XMLSchema-instance";
static char *soap_xsd_ns = "http://www.w3.org/1999/XMLSchema";

/*
  Parameters:
  1- soap_env_ns
  2- soap_env_enc
  3- xsi_ns
  4- xsd_ns
  3- method name
  4- uri
  5- method name(again)
 */
#define SOAP_MSG_TEMPLATE \
  "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"%s\" SOAP-ENV:encoding=\"%s\"" \
  " xmlns:xsi=\"%s\"" \
  " xmlns:xsd=\"%s\">" \
  " <SOAP-ENV:Body>"\
  "  <m:%s xmlns:m=\"%s\">"\
  "  </m:%s>" \
  " </SOAP-ENV:Body>"\
  "</SOAP-ENV:Envelope>"



/*-----------------------------------------------------------------
  FUNCTION: SoapEnvCreate
/-----------------------------------------------------------------*/

HSOAPENV SoapEnvCreate(const char *uri, const char *method)
{
  xmlDocPtr env;
  xmlNodePtr node;
  char buffer[1054];

  const char* FUNC = "SoapEnvCreate";
  
  SoapTraceEnter( FUNC, 
	  "URI = '%s'\nMethod = '%s'",
	  uri, method);

  sprintf(buffer, SOAP_MSG_TEMPLATE, 
	  soap_env_ns, soap_env_enc, soap_xsi_ns, 
	  soap_xsd_ns, method, uri, method);

  env = xmlParseDoc(buffer);
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

  SoapTraceLeave(FUNC, "Envelope: %p", node);
  
  return (HSOAPENV)node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapEnvGetBody
/-----------------------------------------------------------------*/

HSOAPBODY SoapEnvGetBody(HSOAPENV envelope)
{
  xmlNodePtr cur;
  const char* FUNC = "SoapEnvGetBody";
  SoapTraceEnter(FUNC,"");

  if (envelope == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid envelope (null)");
    return NULL;
  }

  cur = ((xmlNodePtr)envelope)->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlStrcmp(cur->name, (const xmlChar*)"Body")) {
      SoapTraceLeave(FUNC, "Found Body (%p)", cur);
      return (HSOAPBODY)cur;
    }
    cur = cur->next;
  }

  SoapTraceLeave(FUNC, "No body found!");
  return NULL;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapEnvFree
/-----------------------------------------------------------------*/

void SoapEnvFree(HSOAPENV envelope)
{
  const char* FUNC = "SoapEnvFree";
  SoapTraceEnter(FUNC,"%p", envelope);

  if (!envelope) {
	  SoapTraceLeave(FUNC, "Envelope is null!");
	  return;
  }

  xmlFreeDoc(((xmlNodePtr)envelope)->doc);
  SoapTraceLeave(FUNC, "Envelope destroyed");
}


/*-----------------------------------------------------------------
  FUNCTION: SoapEnvDump
/-----------------------------------------------------------------*/

void SoapEnvDump(HSOAPENV env)
{
  const char *FUNC = "SoapEnvDump";
  
  xmlBufferPtr buffer;
  xmlDocPtr doc;
  char *str;

  SoapTraceEnter(FUNC, "env (%p)", env);

  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, ((xmlNodePtr)env)->doc, (xmlNodePtr)env, 1 ,1);
  str  = (char*)xmlBufferContent(buffer);

  SoapLog(LOG_DEBUG, FUNC, "%s\n", str?str:"null");

  xmlBufferFree(buffer);
  SoapTraceLeave(FUNC, "Leaving");
}
