/******************************************************************
 *  $Id: csoapfault.c,v 1.2 2003/05/20 21:00:19 snowdrop Exp $
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
#include "csoapfault.h"



/*-----------------------------------------------------------------
  FUNCTION: SoapFaultGetFaultCode
/-----------------------------------------------------------------*/

char* SoapFaultGetFaultCode(HSOAPFAULT fault)
{
  xmlNodePtr node;
  const char *FUNC = "SoapFaultGetFaultCode";

  SoapTraceEnter(FUNC, "Entering");
  
  node = SoapXmlFindNode((HSOAPXML)fault, "faultcode");
  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "Can not find faultcode");
    return NULL;
  }

  SoapTraceLeave(FUNC, "Leaving"); 
  return SoapXmlGetNodeValueAlloc(node);
}


/*-----------------------------------------------------------------
  FUNCTION: SoapFaultGetFaultString
/-----------------------------------------------------------------*/

char* SoapFaultGetFaultString(HSOAPFAULT fault)
{
  xmlNodePtr node;
  const char *FUNC = "SoapFaultGetFaultString";

  SoapTraceEnter(FUNC, "Entering");
  
  node = SoapXmlFindNode((HSOAPXML)fault, "faultstring");
  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "Can not find faultstring");
    return NULL;
  }

  SoapTraceLeave(FUNC, "Leaving"); 
  return SoapXmlGetNodeValueAlloc(node);
}


/*-----------------------------------------------------------------
  FUNCTION: SoapFaultGetFaultActor
/-----------------------------------------------------------------*/

char* SoapFaultGetFaultActor(HSOAPFAULT fault)
{
  xmlNodePtr node;
  const char *FUNC = "SoapFaultGetFaultActor";

  SoapTraceEnter(FUNC, "Entering");
  
  node = SoapXmlFindNode((HSOAPXML)fault, "faultactor");
  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "Can not find faultactor");
    return NULL;
  }

  SoapTraceLeave(FUNC, "Leaving"); 
  return SoapXmlGetNodeValueAlloc(node);
}


/*-----------------------------------------------------------------
  FUNCTION: SoapFaultGetFaultDetail
/-----------------------------------------------------------------*/

HSOAPPARAM SoapFaultGetDetail(HSOAPFAULT fault)
{
  xmlNodePtr node;
  const char *FUNC = "SoapFaultGetFaultCode";

  SoapTraceEnter(FUNC, "Entering");
  
  node = SoapXmlFindNode((HSOAPXML)fault, "detail");
  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "Can not find detail");
    return NULL;
  }

  SoapTraceLeave(FUNC, "Leaving"); 
  return (HSOAPPARAM)node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapFaultPrint
/-----------------------------------------------------------------*/

void SoapFaultPrint(FILE* f, HSOAPFAULT fault)
{
	char *str;

	if (fault == NULL) {
		fprintf(f, "Fault object is null!.\n");
		return;
	}

	str = SoapFaultGetFaultCode(fault);
	fprintf(f, "FaultCode: %s\n", str?str:"null");
	SoapFreeStr(str);

	str = SoapFaultGetFaultString(fault);
	fprintf(f, "FaultString: %s\n", str?str:"null");
	SoapFreeStr(str);

	str = SoapFaultGetFaultActor(fault);
	fprintf(f, "FaultActor: %s\n", str?str:"null");
	SoapFreeStr(str);
}
