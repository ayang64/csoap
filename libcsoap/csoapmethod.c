/******************************************************************
 *  $Id: csoapmethod.c,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#include "libcsoap/csoapmethod.h"
#include "libcsoap/csoaplog.h"


/*-----------------------------------------------------------------
  FUNCTION:SoapMethodGetName
/-----------------------------------------------------------------*/

char* SoapMethodGetName(HSOAPMETHOD method)
{
  const char* FUNC = "SoapMethodGetName";
  char *name;
  xmlNodePtr node;
  SoapTraceEnter(FUNC,"");

  if (method == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid method (null)");
    return NULL;
  }
  node = (xmlNodePtr)method;
  name = (char*)node->name;
  SoapTraceLeave(FUNC, "Name = '%s'", name);
  return name;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapMethodParamContext
/-----------------------------------------------------------------*/

HSOAPPARAM SoapMethodParamContext(HSOAPMETHOD method)
{
  const char* FUNC = "SoapMethodParamContext";
  char *name;
  SoapTraceEnter(FUNC,"");

  if (method == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid method (null)");
    return NULL;
  }
  SoapTraceLeave(FUNC, "param context = '%p'", method);
  
  return (xmlNodePtr)method;
}

