/******************************************************************
 *  $Id: soap-xml.c,v 1.2 2004/02/03 08:07:36 snowdrop Exp $
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
#include <libcsoap/soap-xml.h>


xmlNodePtr soap_xml_get_children(xmlNodePtr param)
{
  xmlNodePtr children;

  if (param == NULL) {
    log_error1("Invalid parameter 'param' (null)");
    return NULL;
  }

  children = param->xmlChildrenNode;
   while (children != NULL) {
    if (children->type != XML_ELEMENT_NODE)
      children = children->next;
    else break;
  }
  
  return children;
}

xmlNodePtr soap_xml_get_next(xmlNodePtr param)
{

  xmlNodePtr node = param->next;

  while (node != NULL) {
    if (node->type != XML_ELEMENT_NODE)
      node = node->next;
    else break;
  }

  return node;
}


xmlXPathObjectPtr 
soap_xpath_eval(xmlDocPtr doc, const char *xpath)
{
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);
  result = xmlXPathEvalExpression((xmlChar*)xpath, context);
  if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
    /* no result */
    return NULL;
  }

  xmlXPathFreeContext(context);
  return result;  
}


int
soap_xpath_foreach(xmlDocPtr doc, const char *xpath, 
		   soap_xmlnode_callback cb)
{
  int i = 0;
  xmlNodeSetPtr nodeset; 
  xmlXPathObjectPtr xpathobj;
  
  xpathobj = soap_xpath_eval(doc, xpath);

  if (!xpathobj) return 0;

  nodeset = xpathobj->nodesetval;
  if (!nodeset) return 0;

  for (i=0;i < nodeset->nodeNr; i++) {
    if (!cb(nodeset->nodeTab[i]))
      break;
  }
  
  xmlXPathFreeObject(nodeset);
  return i;
}


void soap_xml_doc_print(xmlDocPtr doc)
{
  xmlBufferPtr buffer;
  xmlNodePtr root;

  if (doc == NULL) {
    puts("xmlDocPtr is NULL!");
    return;
  }

  root = xmlDocGetRootElement(doc);
  if (root == NULL) {
    puts("Empty document!");
    return;
  }
  

  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, doc, root, 1 ,1);
  puts( (const char*)xmlBufferContent(buffer));
  xmlBufferFree(buffer);  

}
