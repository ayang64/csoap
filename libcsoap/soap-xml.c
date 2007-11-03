/******************************************************************
*  $Id: soap-xml.c,v 1.14 2007/11/03 22:40:10 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/tree.h>

#include "soap-logging.h"
#include "soap-xml.h"

xmlNodePtr
soap_xml_get_children(xmlNodePtr node)
{
  xmlNodePtr child;

  if (node == NULL)
  {
    log_error("Invalid node (null)");
    return NULL;
  }

  for (child = node->children; child; child=child->next)
  {
    if (child->type == XML_ELEMENT_NODE)
    {
      return child;
    }
  }

  return NULL;
}

xmlNodePtr
soap_xml_get_next_element(xmlNodePtr param)
{

  if (param == NULL)
  {
    log_error("Invalid node (null)");
    return NULL;
  }

  xmlNodePtr node = param->next;

  while (node != NULL)
  {
    if (node->type != XML_ELEMENT_NODE)
      node = node->next;
    else
      break;
  }

  return node;
}

char *
soap_xml_get_text(xmlNodePtr node)
{
  return (char *) xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
}
