/******************************************************************
 *  $Id: csoapparam.c,v 1.1 2003/03/25 22:17:25 snowdrop Exp $
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
#include "csoapparam.h"
#include "csoaplog.h"
#include "csoaptypes.h"
#include "csoapxml.h"

#include <stdarg.h>


/*-----------------------------------------------------------------
  FUNCTION: SoapParamCreate
/-----------------------------------------------------------------*/

HSOAPPARAM  SoapParamCreate(HSOAPPARAM parent, const char *type,
			     const char *ns, const char *name,
			     const char *format,  ...)
{
  va_list ap;
  char buffer[SOAP_MAX_STRING_BUFFER];
  xmlNodePtr newnode;
  const char *FUNC = "SoapParamCreate";

  SoapTraceEnter(FUNC,"");

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  newnode = xmlNewTextChild(parent, NULL, name, buffer);

  if (newnode == NULL) {
    SoapTraceLeaveWithError(FUNC,
			    "Can not create new xml node");
    return NULL;
  }

  if ( !xmlNewProp(newnode, "xsi:type", type)) {
    SoapTraceLeaveWithError(FUNC,
			    "Can not create new xml attribute");
    return NULL;
  }

  SoapTraceLeave(FUNC, "New xml node (%p)", newnode);
  return newnode;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamCreateString
/-----------------------------------------------------------------*/

HSOAPPARAM SoapParamCreateString(HSOAPPARAM parent, const char *name,
				 const char *format,  ...)
{
  va_list ap;
  HSOAPPARAM node;
  char buffer[SOAP_MAX_STRING_BUFFER];
  const char* FUNC = "SoapParamCreateString";
  SoapTraceEnter(FUNC,"");

  if (parent == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parent (null)");
    return NULL;
  }

  if (name == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid name (null)");
    return NULL;
  }

  if (format == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid format (null)");
    return NULL;
  }

  SoapLog(LOG_DEBUG, FUNC, 
	  "Creating string parameter\n" \
	  "Name = '%s'" \
	  "Format = %s", name, format);

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  node = SoapParamCreate(parent ,  SOAP_TYPE_STRING,
			  NULL, name, "%s", buffer);

  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "can not create param");
    return NULL;
  }

  SoapTraceLeave(FUNC, "New param (%p)", node);
  return (HSOAPPARAM)node;
}



/*-----------------------------------------------------------------
  FUNCTION: SoapParamCreateInt
/-----------------------------------------------------------------*/

HSOAPPARAM SoapParamCreateInt(HSOAPPARAM parent, const char *name,
			      int value)
{
  HSOAPPARAM node;
  const char* FUNC = "SoapParamCreateInt";
  SoapTraceEnter(FUNC,"");

  if (parent == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parent (null)");
    return NULL;
  }

  if (name == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid name (null)");
    return NULL;
  }  

  node = SoapParamCreate(parent , SOAP_TYPE_INT,
			  NULL, name, "%d", value);

  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "can not create param");
    return NULL;
  }

  SoapTraceLeave(FUNC, "New param (%p)", node);
  return node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamCreateDouble
/-----------------------------------------------------------------*/

HSOAPPARAM SoapParamCreateDouble(HSOAPPARAM parent, const char *name,
				 double value)
{
  HSOAPPARAM node;
  const char* FUNC = "SoapParamCreateDouble";
  SoapTraceEnter(FUNC,"");

  if (parent == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parent (null)");
    return NULL;
  }

  if (name == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid name (null)");
    return NULL;
  }  

  node = SoapParamCreate(parent, SOAP_TYPE_DOUBLE,
			  NULL, name, "%f", value);

  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC, "can not create param");
    return NULL;
  }

  SoapTraceLeave(FUNC, "New param (%p)", node);
  return node;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamSetValue
/-----------------------------------------------------------------*/

void SoapParamSetValue(HSOAPPARAM param, const char* type,
		       const char* format, ...)
{
  // Not implemented yet!
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetString
/-----------------------------------------------------------------*/

int SoapParamGetString(HSOAPPARAM param, char *dest)
{
  char *value;
  int len;
  xmlNodePtr xmlNode;
  const char* FUNC = "SoapParamGetString";
  SoapTraceEnter(FUNC,"");

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC,
	    "Invalid parameter 'param' (null)");
    return 0;
  }

  xmlNode = (xmlNodePtr)param;
  value = (char*)xmlNodeListGetString(xmlNode->doc,
			       xmlNode->xmlChildrenNode,1);
  
  if (dest)
    strcpy(dest, value);

  len = strlen(value);
  xmlFree(value);

  SoapTraceLeave(FUNC, "Value = '%s'\nLength=%d", dest?dest:"null", len);
  return len;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetInt
/-----------------------------------------------------------------*/

int SoapParamGetInt(HSOAPPARAM param)
{
  int value;
  char buffer[15];
  const char* FUNC = "SoapParamGetInt";
  SoapTraceEnter(FUNC,"");

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parameter 'param' (null)");
    return 0;
  }

  SoapParamGetString(param, buffer);
  value = atoi(buffer);
  SoapTraceLeave(FUNC, "Value = %d", value);
  return value;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetDouble
/-----------------------------------------------------------------*/

double SoapParamGetDouble(HSOAPPARAM param)
{
  double value;
  char buffer[15];
  const char* FUNC = "SoapParamGetDouble";
  SoapTraceEnter(FUNC,"");

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parameter 'param' (null)");
    return 0.0;
  }

  SoapParamGetString(param, buffer);
  value = atof(buffer);
  SoapTraceLeave(FUNC, "Value = %f", value);
  return value;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetChildren
/-----------------------------------------------------------------*/

HSOAPPARAM SoapParamGetChildren(HSOAPPARAM param)
{
  xmlNodePtr children;
  const char* FUNC = "SoapParamGetChildren";
  SoapTraceEnter(FUNC,"");

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parameter 'param' (null)");
    return NULL;
  }

  children = ((xmlNodePtr)param)->xmlChildrenNode;
   while (children != NULL) {
    if (children->type != XML_ELEMENT_NODE)
      children = children->next;
    else break;
  }
  SoapTraceLeave(FUNC, "Children node (%p)", children);
  return (HSOAPPARAM)children;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetNext
/-----------------------------------------------------------------*/

HSOAPPARAM SoapParamGetNext(HSOAPPARAM param)
{
  xmlNodePtr children;
  const char* FUNC = "SoapParamGetNext";
  xmlNodePtr node = (xmlNodePtr)param;
  SoapTraceEnter(FUNC,"Enter. node = %s", node?(char*)node->name:"null");

  if (param == NULL) {
    SoapTraceLeaveWithError(FUNC, 
	    "Invalid parameter 'param' (null)");
    return NULL;
  }

  children = node->next;

  while (children != NULL) {
    if (children->type != XML_ELEMENT_NODE)
      children = children->next;
    else break;
  }

  SoapTraceLeave(FUNC, "Children node (%p)", children);
  return (HSOAPPARAM)children;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamGetType
/-----------------------------------------------------------------*/

char* SoapParamGetType(HSOAPPARAM param)
{
  const char* FUNC = "SoapParamGetType";
  char *value;
  xmlNodePtr node = (xmlNodePtr)param;
  SoapTraceEnter(FUNC,"Enter. node = %s", node?(char*)node->name:"null");

  value = SoapXmlGetAttr(node, NULL, "type");

  SoapTraceLeave(FUNC, "Type = %s", value?value:"(null}");
  return value;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapParamTypeIs
/-----------------------------------------------------------------*/

int SoapParamTypeIs(HSOAPPARAM param, const char* type)
{
  const char* FUNC = "SoapParamTypeIs";
  char *value;
  SoapTraceEnter(FUNC,"Enter. Type = %s", type?type:"(null)");

  if (type == NULL) {
    SoapTraceLeaveWithError(FUNC, "type is null");
    return 0;
  }

  value = SoapParamGetType(param);
  
  SoapTraceLeave(FUNC, "Leave");
  return (!strcmp(value, type));
}


