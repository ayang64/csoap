/******************************************************************
 *  $Id: csoapcall.c,v 1.2 2003/05/20 21:00:19 snowdrop Exp $
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
#include "csoapcall.h"
#include "csoaplog.h"
#include "csoaphttp.h"

#include <stdarg.h>


HSOAPCALL SoapCallCreate(
   const char* url,
   const char* uri,
   const char* method)
{
  const char *FUNC = "SoapCallCreate";
  HSOAPCALL call;

  SoapTraceEnter(FUNC, "url=%s;uri=%s;method=%s", 
	  url, uri, method);
  call = (HSOAPCALL)malloc(sizeof(struct SOAPCALL));
  call->m_method = (char*)malloc(strlen(method)+1);

  call->m_env = SoapEnvCreate(uri, method);
  call->m_url = SoapUrlCreate(url);
  strcpy(call->m_method, method);

  SoapTraceLeave(FUNC, "call (%p)", call);
  return call;
}


HSOAPPARAM SoapCallAddParam(
   HSOAPCALL call,
   const char* name,
   const char* type,
   const char* format,
   ...)
{
  const char *FUNC = "SoapCallAddParam";
  HSOAPBODY body;
  HSOAPMETHOD method;
  HSOAPPARAM param, result;
  va_list ap;
  char buffer[SOAP_MAX_STRING_BUFFER];

  SoapTraceEnter(FUNC, "name=%s;type=%s;format=%s", 
	  name, type, format);

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);
  
  body = SoapEnvGetBody(call->m_env);
  method = SoapBodyGetMethod(body, call->m_method);
  param = SoapMethodParamContext(method);
  result = SoapParamCreate(param, type, NULL, name, "%s", buffer);

  SoapTraceLeave(FUNC, "created param (%p)", result);
  return result;
}
   

HSOAPPARAM SoapCallAddStringParam(
   HSOAPCALL call,
   const char* name,
   const char* format,
   ...)
{
  const char *FUNC = "SoapCallAddStringParam";
  HSOAPBODY body;
  HSOAPMETHOD method;
  HSOAPPARAM param, result;
  va_list ap;
  char buffer[SOAP_MAX_STRING_BUFFER];

  SoapTraceEnter(FUNC, "name=%s;format=%s", 
	  name, format);

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);
  
  body = SoapEnvGetBody(call->m_env);
  method = SoapBodyGetMethod(body, call->m_method);
  param = SoapMethodParamContext(method);
  result = SoapParamCreateString(param, name, buffer);
  SoapTraceLeave(FUNC, "created param (%p)", result);
  return result;
}


HSOAPPARAM SoapCallAddIntParam(
   HSOAPCALL call,
   const char* name,
   int value)
{
  const char *FUNC = "SoapCallAddIntParam";
  HSOAPBODY body;
  HSOAPMETHOD method;
  HSOAPPARAM param, result;

  SoapTraceEnter(FUNC, "name=%s;value=%d", 
	  name, value);
  
  body = SoapEnvGetBody(call->m_env);
  method = SoapBodyGetMethod(body, call->m_method);
  param = SoapMethodParamContext(method);
  result = SoapParamCreateInt(param, name, value);
  SoapTraceLeave(FUNC, "created param (%p)", result);
  return result;
}

HSOAPPARAM SoapCallAddDoubleParam(
   HSOAPCALL call,
   const char* name,
   double value)
{
  const char *FUNC = "SoapCallAddDoubleParam";
  HSOAPBODY body;
  HSOAPMETHOD method;
  HSOAPPARAM param, result;

  SoapTraceEnter(FUNC, "name=%s;value=%f", 
	  name, value);
  
  body = SoapEnvGetBody(call->m_env);
  method = SoapBodyGetMethod(body, call->m_method);
  param = SoapMethodParamContext(method);
  result = SoapParamCreateDouble(param, name, value);
  SoapTraceLeave(FUNC, "created param (%p)", result);
  return result;
}


HSOAPRES SoapCallInvoke(HSOAPCALL call)
{
  const char* FUNC = "SoapCallInvoke";
  SoapTraceEnter(FUNC,"%p", call);
  const char* filename = "csoap-client.xml";
  char *buf;
  CSOAP_STATUS status;
  HSOAPENV env;
  HSOAPRES res;

  if (!call) {
	  SoapTraceLeaveWithError(FUNC, "Call is null!");
	  return NULL;
  }

  if (!call->m_env) {
	  SoapTraceLeaveWithError(FUNC, "Invalid envelope!");
	  return NULL;
  }

  if (!call->m_url) {
    SoapTraceLeaveWithError(FUNC, "Invalid URL");
    return NULL;
  }

/*  xmlSaveFormatFile(filename, ((xmlNodePtr)call->m_env)->doc, 1);
  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, ((xmlNodePtr)call->m_env)->doc,
	  (xmlNodePtr)call->m_env, 1 ,2);
  printf("\n%s\n", (const char*)xmlBufferContent(buffer));
  xmlBufferFree(buffer);
*/

  printf("Sending ...\n");
  status = SoapHttpSend(call->m_url, call->m_env, &env);
  if (status != CSOAP_OK) {
    printf("can not send. code = %d\n", status);
  }
  printf("Sending finished\n");
  /*
    printf("Length: %d\nResponse: \n%s\n\n\r", strlen(buf), buf?buf:"null");
  */
  SoapEnvDump(env); 
  /*  free(buf);*/
  /*  SoapEnvFree(env);*/
  res = SoapResCreate(env, call->m_method);
  SoapTraceLeave(FUNC, "res (%p)", res);
  return res;
}


void SoapCallFree(HSOAPCALL call)
{
  const char* FUNC = "SoapCallFree";
  SoapTraceEnter(FUNC,"%p", call);

  if (!call) {
    SoapTraceLeave(FUNC, "Call is null!");
    return;
  }

  SoapEnvFree(call->m_env);

  if (call->m_url) SoapUrlFree(call->m_url);
  if (call->m_method) free(call->m_method);

  free(call);
  SoapTraceLeave(FUNC, "Call destroyed");
}

