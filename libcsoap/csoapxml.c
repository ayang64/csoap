/******************************************************************
 *  $Id: csoapxml.c,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#include "libcsoap/csoapxml.h"

#include "libcsoap/csoaplog.h"


/*-----------------------------------------------------------------
  FUNCTION: SoapXmlFindNode
/-----------------------------------------------------------------*/

HSOAPXML 
SoapXmlFindNode(HSOAPXML root, const char* nodeName)
{
  xmlNodePtr cur;
  const char* FUNC = "SoapXmlFindNode";

  SoapTraceEnter(FUNC,"root (%p)  node (%p)", 
		 root, nodeName?nodeName:"null");

  if (root == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid root node (null)");
    return NULL;
  }

  if (nodeName == NULL) {
    SoapLog(LOG_ERROR, FUNC, 
	    "Invalid node name (null)");
    return NULL;
  }

  cur = ((xmlNodePtr)root)->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlStrcmp(cur->name, (const xmlChar*)nodeName)) {
      SoapTraceLeave(FUNC, "Found '%s' (%p)", nodeName, cur);
      return cur;
    }
    cur = cur->next;
  }

  SoapTraceLeave(FUNC, "No '%s' found!", nodeName);
  return NULL;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapXmlGetNodeValue
/-----------------------------------------------------------------*/

int
SoapXmlGetNodeValue(HSOAPXML xmlNode, char* dest)
{
  char *value;
  int len;
  const char* FUNC = "SoapXmlGetNodeValue";
  SoapTraceEnter(FUNC,"Entering");


  if (xmlNode == NULL) {
    SoapTraceLeaveWithError(FUNC,
	    "Invalid parameter 'xmlNode' (null)");
    return 0;
  }

  value = (char*)xmlNodeListGetString(((xmlNodePtr)xmlNode)->doc,
			       ((xmlNodePtr)xmlNode)->xmlChildrenNode,1);
  strcpy(dest, value);
  len = strlen(value);
  xmlFree(value);

  SoapTraceLeave(FUNC, "Value = '%s'\nLength=%d", dest, len);
  return len;

}


/*-----------------------------------------------------------------
  FUNCTION: SoapXmlGetNodeValueAlloc
/-----------------------------------------------------------------*/

char*
SoapXmlGetNodeValueAlloc(HSOAPXML xmlNode)
{
  char *value;
  int len;
  const char* FUNC = "SoapXmlGetNodeValueAlloc";
  SoapTraceEnter(FUNC,"Entering");


  if (xmlNode == NULL) {
    SoapTraceLeaveWithError(FUNC,
	    "Invalid parameter 'xmlNode' (null)");
    return 0;
  }

  value = (char*)xmlNodeListGetString(((xmlNodePtr)xmlNode)->doc,
			       ((xmlNodePtr)xmlNode)->xmlChildrenNode,1);
  SoapTraceLeave(FUNC, "Value = '%s'\n", value);
  return value;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapXmlGetAttr
/-----------------------------------------------------------------*/

char* SoapXmlGetAttr(HSOAPXML node, const char* ns, const char* key)
{
  char *value;
  int len;
  const char* FUNC = "SoapXmlGetAttr";
  SoapTraceEnter(FUNC,"Entering");


  if (node == NULL) {
    SoapTraceLeaveWithError(FUNC,
	    "Invalid parameter 'node' (null)");
    return NULL;
  }
  
  if (ns == NULL) {
    value = (char*)xmlGetProp(node, key);
  } else {
    value = (char*)xmlGetNsProp(node, key, ns);
  }

  SoapTraceLeave(FUNC, "Value = '%s'\n", value?value:"(null)");
  return value;
}
