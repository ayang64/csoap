/******************************************************************
 *  $Id: csoapxml.h,v 1.1 2003/03/25 22:17:33 snowdrop Exp $
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
#ifndef CSOAP_XML_H
#define CSOAP_XML_H


#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "csoaperror.h"


typedef xmlNodePtr HSOAPXML;


/**
 * Search for an xml node which
 * has the name nodeName
 *
 * @param root the root xml node to search
 *   the nodeName in its children nodes
 * @param nodeName the nodeName to find
 *
 * @return an xml node if any exist, 
 *	NULL otherwise
 */
HSOAPXML SoapXmlFindNode(HSOAPXML root, const char* nodeName);


/**
 * Receives the node value
 *
 * @param node the xml node to receive 
 *  its text node
 * @param dest the destination string
 *
 * @return length of content copied into dest
 */
int  SoapXmlGetNodeValue(HSOAPXML node, char* dest);


/**
 * Receives the node value.
 *
 * @param node the xml node to receive 
 *  its text node
 *
 * @return An allocated string. You must free
 *  it using <B>SoapFreeStr</B>
 * @see SoapFreeStr
 */
char*  SoapXmlGetNodeValueAlloc(HSOAPXML node);


/**
 * Returns the xml attribute
 *
 * @param node the xml node to receive its attribute
 * @param ns namespace of the attribute
 * @param key key of the attribute
 *
 * @return value of the attribute if any found,
 *   NULL otherwise. It's up to the caller to free 
 *   the memory with SoapFreeStr().
 */
char* SoapXmlGetAttr(HSOAPXML node, const char* ns, const char* key);

#endif

