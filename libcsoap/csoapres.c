/******************************************************************
 *  $Id: csoapres.c,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#include "libcsoap/csoapres.h"
#include "libcsoap/csoaplog.h"


/*-----------------------------------------------------------------
  FUNCTION: SoapResCreate
/-----------------------------------------------------------------*/

HSOAPRES SoapResCreate(HSOAPENV env, const char* methodName)
{
  HSOAPRES res = (HSOAPRES)malloc(sizeof(struct SOAPRES));
  res->m_env = env;
  res->m_method = (char*)malloc(sizeof(char)*strlen(methodName)+1);
  strcpy(res->m_method, methodName);
  return res;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapResGetFault
/-----------------------------------------------------------------*/

HSOAPFAULT SoapResGetFault(HSOAPRES res)
{
  const char *FUNC = "SoapResGetFault";
  HSOAPBODY body;
  HSOAPFAULT fault;

  SoapTraceEnter(FUNC, "Entering");

  if (res == NULL) {
    SoapTraceLeaveWithError(FUNC, "res is null");
    return NULL;
  }

  if (res->m_env == NULL) {
    SoapTraceLeaveWithError(FUNC, "res->m_env is null");
    return NULL;
  }

  if (!(body = SoapEnvGetBody(res->m_env))) {
    SoapTraceLeaveWithError(FUNC, "can not get body");
    return NULL;
  }

  fault = SoapBodyGetFault(body);
  SoapTraceLeave(FUNC, "Leaving fault (%p)", fault);

  return fault;
}

/*-----------------------------------------------------------------
  FUNCTION: SoapResFree
/-----------------------------------------------------------------*/

void SoapResFree(HSOAPRES res)
{
  const char *FUNC = "SoapResFree";

  SoapTraceEnter(FUNC, "Entering");

  if (res == NULL) {
    SoapTraceLeaveWithError(FUNC, "res is null");
    return;
  }

  if (res->m_env == NULL) {
    SoapTraceLeaveWithError(FUNC, "res->m_env is null");
    return;
  }

  SoapEnvFree(res->m_env);
  free(res);

  SoapTraceLeave(FUNC, "Leaving");
}


/*-----------------------------------------------------------------
  FUNCTION: SoapResFree
/-----------------------------------------------------------------*/

HSOAPPARAM SoapResGetParamChildren(HSOAPRES res)
{
  const char *FUNC = "SoapResGetParamChildren";
  HSOAPBODY body;
  HSOAPMETHOD method;
  HSOAPPARAM param;
  char tmp[1054];

  SoapTraceEnter(FUNC, "Entering");

  if (res == NULL) {
    SoapTraceLeaveWithError(FUNC, "res is null");
    return NULL;
  }

  if (res->m_env == NULL) {
    SoapTraceLeaveWithError(FUNC, "res->m_env is null");
    return NULL;
  }

  if (!(body = SoapEnvGetBody(res->m_env))) {
    SoapTraceLeaveWithError(FUNC, "can not get body");
    return NULL;
  }

  sprintf(tmp, "%sResponse", res->m_method);
  method = SoapBodyGetMethod(body, tmp);

  if (method == NULL) {
    SoapTraceLeaveWithError(FUNC, "method is null");
    return NULL;
  }

  param = SoapMethodParamContext(method);

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, "param is null");
    return NULL;
  }

  param = SoapParamGetChildren(method);

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, "child is null");
    return NULL;
  }

  return param;
}
