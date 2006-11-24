/******************************************************************
*  $Id: soap-addressing.c,v 1.4 2006/11/24 10:54:03 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2006 Heiko Ronsdorf
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
* Email: hero@persua.de
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UUID_H
#include <uuid.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-xml.h"
#include "soap-fault.h"
#include "soap-env.h"
#include "soap-addressing.h"

static const xmlChar *
_soap_addressing_uuid_error(uint32_t status)
{
  switch(status)
  {
    case uuid_s_bad_version:
      return "The UUID does not have a known version";
    case uuid_s_invalid_string_uuid:
      return "The string representation of an UUID is not valid";
    case uuid_s_no_memory:
      /* XXX: From FreeBSD 6.2 UUID(3) ??? */  
      return "The meaning of the code escaped the writers mind";
    default:
      return "Unkown error";
  }
}

static char *
_soap_addressing_generate_id(void)
{
  uuid_t uuid;
  uint32_t status;
  char *ret, *buf;

  uuid_create(&uuid, &status);
  if (status != uuid_s_ok)
  {
    log_error2("uuidcreate failed (%s)", _soap_addressing_uuid_error(status));
    return NULL;
  }

  uuid_to_string(&uuid, &buf, &status);
  if (status != uuid_s_ok)
  {
    log_error2("uuid_to_string failed (%s)", _soap_addressing_uuid_error(status));
    return NULL;
  }

  if (!(ret = (char *)malloc(128)))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    free(buf);
    return NULL;
  }

  sprintf(ret, "%s/%s", soap_server_get_name(), buf);

  free(buf);

  return ret;
}

static xmlNsPtr
_soap_addressing_get_namespace(xmlNodePtr node)
{
  xmlNsPtr ns;

  if (!(ns = xmlSearchNsByHref(node->doc, node, BAD_CAST WSA_NAMESPACE)))
  {
    ns = xmlNewNs(node, BAD_CAST WSA_NAMESPACE, BAD_CAST WSA_NAMESPACE_PREFIX);
  }
  return ns;
}

static xmlNodePtr
_soap_addressing_add_node(xmlNodePtr parent, const xmlChar *name, const xmlChar *content)
{
  xmlNsPtr ns;

  ns = _soap_addressing_get_namespace(parent);
  return xmlNewChild(parent, ns, name, content);
}

static xmlAttrPtr
_soap_addressing_set_property(xmlNodePtr node, const xmlChar *name, const xmlChar *value)
{
  xmlNsPtr ns;

  ns = _soap_addressing_get_namespace(node);
  return xmlSetNsProp(node, ns, name, value);
}

static xmlAttrPtr
_soap_addressing_set_property_uri(xmlNodePtr node, const xmlChar *name, xmlURI *uri)
{
  xmlChar *buf;
  xmlAttrPtr ret;

  buf = xmlSaveUri(uri);
  ret = _soap_addressing_set_property(node, name, buf);
  xmlFree(buf);

  return ret;
}

static xmlNodePtr
_soap_addressing_get_child_element(xmlNodePtr parent, const xmlChar *name)
{
  xmlNodePtr walker;

  for (walker = soap_xml_get_children(parent); walker; walker = soap_xml_get_next(walker))
  {
    if (!xmlStrcmp(walker->name, name) && !xmlStrcmp(walker->ns->href, WSA_NAMESPACE))
      return walker;
  }
  return NULL;
}

static xmlURI *
_soap_addressing_extract_uri(xmlNodePtr node)
{
  xmlChar *content;
  xmlURI *uri = NULL;

  if ((content = xmlNodeGetContent(node)))
  {
    uri = xmlParseURI(content);
    xmlFree(content);
  }
  return uri;
}

static xmlNodePtr
_soap_addressing_set_content_uri(xmlNodePtr node, xmlURI *uri)
{
  xmlChar *buf;

  if (uri == NULL)
    return;

  buf = xmlSaveUri(uri);
  xmlNodeSetContent(node, buf);
  xmlFree(buf);

  return node;
}

xmlURI *
soap_addressing_get_address(xmlNodePtr endpoint_reference)
{
  xmlNodePtr address;
  
  address = _soap_addressing_get_child_element(endpoint_reference, WSA_ADDRESS);
  if (address == NULL)
    return NULL;

  return _soap_addressing_extract_uri(address);
}

xmlNodePtr
soap_addressing_set_address(xmlNodePtr endpoint_reference, xmlURI *address)
{
  xmlNodePtr node;

  node = _soap_addressing_get_child_element(endpoint_reference, WSA_ADDRESS);
  if (node == NULL)
    node = _soap_addressing_add_node(endpoint_reference, WSA_ADDRESS, NULL);

  return _soap_addressing_set_content_uri(node, address);  
}

xmlNodePtr
soap_addressing_get_reference_properties(xmlNodePtr endpoint_reference)
{
  return _soap_addressing_get_child_element(endpoint_reference, WSA_REFERENCE_PROPERTIES);
}

xmlNodePtr
soap_addressing_set_reference_properties(xmlNodePtr endpoint_reference, xmlNodePtr properties)
{
  xmlNodePtr node;

  node = soap_addressing_get_reference_properties(endpoint_reference);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  node = xmlCopyNode(properties, 1);
  xmlUnlinkNode(node);
  xmlAddChild(endpoint_reference, node);

  return node;
}

xmlNodePtr
soap_addressing_get_metadata(xmlNodePtr endpoint_reference)
{
  return _soap_addressing_get_child_element(endpoint_reference, WSA_METADATA);
}

xmlNodePtr
soap_addressing_set_metadata(xmlNodePtr endpoint_reference, xmlNodePtr metadata)
{
  xmlNodePtr node;

  node = soap_addressing_get_metadata(endpoint_reference);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  node = xmlCopyNode(metadata, 1);
  xmlUnlinkNode(node);
  xmlAddChild(endpoint_reference, node);

  return node;
}

xmlURI *
soap_addressing_get_message_id(struct SoapEnv *envelope)
{
  xmlNodePtr id;

  id = _soap_addressing_get_child_element(envelope->header, WSA_MESSAGE_ID);
  if (id == NULL)
    return NULL;

  return _soap_addressing_extract_uri(id);
}

xmlNodePtr
soap_addressing_set_message_id(struct SoapEnv *envelope, xmlURI *id)
{
  xmlNodePtr node;

  node = _soap_addressing_get_child_element(envelope->header, WSA_MESSAGE_ID);
  if (node == NULL)
    node = _soap_addressing_add_node(envelope->header, WSA_MESSAGE_ID, NULL);

  return _soap_addressing_set_content_uri(node, id);
}

xmlNodePtr
soap_addressing_set_message_id_string(struct SoapEnv *envelope, xmlChar *id)
{
  xmlNodePtr node;
  xmlChar *tmp;

  if (id == NULL)
    tmp = _soap_addressing_generate_id();
  else
    tmp = id;

  log_verbose2("setting message id = \"%s\"", tmp);

  node = _soap_addressing_get_child_element(envelope->header, WSA_MESSAGE_ID);
  if (node == NULL)
    node = _soap_addressing_add_node(envelope->header, WSA_MESSAGE_ID, tmp);
  else
    xmlNodeSetContent(node, tmp);

  if (id == NULL)
    xmlFree(tmp);

  return node;
}

xmlChar *soap_addressing_get_message_id_string(struct SoapEnv *envelope)
{
  xmlURI *uri;
  xmlChar *ret;

  if (!(uri = soap_addressing_get_message_id(envelope)))
    return NULL;

  ret = xmlSaveUri(uri);
  xmlFreeURI(uri);

  return ret;
}

xmlNodePtr
soap_addressing_get_relates_to(struct SoapEnv *envelope)
{
  return _soap_addressing_get_child_element(envelope->header, WSA_RELATES_TO);
}

xmlNodePtr
soap_addressing_add_relates_to(struct SoapEnv *envelope, xmlURI *id, xmlURI *type)
{
  xmlNodePtr node;

  node = soap_addressing_get_relates_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  node = _soap_addressing_add_node(envelope->header, WSA_RELATES_TO, NULL);
  _soap_addressing_set_content_uri(node, id);
  _soap_addressing_set_property_uri(node, WSA_RELATIONSHIP_TYPE, type);

  return node;
}

xmlNodePtr
soap_addressing_get_reply_to(struct SoapEnv *envelope)
{
  return _soap_addressing_get_child_element(envelope->header, WSA_REPLY_TO);
}

xmlNodePtr
soap_addressing_set_reply_to(struct SoapEnv *envelope, xmlNodePtr address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_reply_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_REPLY_TO, NULL);
  node = xmlCopyNode(address, 1);
  xmlUnlinkNode(node);
  xmlAddChild(ret, node);

  return ret;
}

xmlURI *
soap_addressing_get_reply_to_address(struct SoapEnv *envelope)
{
  xmlNodePtr reply_to;

  reply_to = soap_addressing_get_reply_to(envelope);
  if (reply_to == NULL)
    return NULL;

  return soap_addressing_get_address(reply_to);
}

xmlNodePtr
soap_addressing_set_reply_to_address(struct SoapEnv *envelope, xmlURI *address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_reply_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_REPLY_TO, NULL);
  soap_addressing_set_address(ret, address);

  return ret;
}

xmlNodePtr
soap_addressing_get_from(struct SoapEnv *envelope)
{
  return _soap_addressing_get_child_element(envelope->header, WSA_FROM);
}

xmlNodePtr
soap_addressing_set_from(struct SoapEnv *envelope, xmlNodePtr address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_from(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_FROM, NULL);
  node = xmlCopyNode(address, 1);
  xmlUnlinkNode(node);
  xmlAddChild(ret, node);

  return ret;
}

xmlNodePtr
soap_addressing_set_from_string(struct SoapEnv *envelope, const char *from)
{
  xmlURI *uri;
  xmlNodePtr ret;

  uri = xmlParseURI(from);
  ret = soap_addressing_set_from_address(envelope, uri);
  xmlFreeURI(uri);

  return ret;
}

xmlURI *
soap_addressing_get_from_address(struct SoapEnv *envelope)
{
  xmlNodePtr from;

  from = soap_addressing_get_from(envelope);
  if (from == NULL)
    return NULL;

  return soap_addressing_get_address(from);
}

xmlNodePtr
soap_addressing_set_from_address(struct SoapEnv *envelope, xmlURI *address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_from(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_FROM, NULL);
  soap_addressing_set_address(ret, address);

  return ret;
}

xmlNodePtr
soap_addressing_get_fault_to(struct SoapEnv *envelope)
{
  return _soap_addressing_get_child_element(envelope->header, WSA_FAULT_TO);
}

xmlNodePtr
soap_addressing_set_fault_to(struct SoapEnv *envelope, xmlNodePtr address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_fault_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_FAULT_TO, NULL);
  node = xmlCopyNode(address, 1);
  xmlUnlinkNode(node);
  xmlAddChild(ret, node);

  return ret;
}

xmlURI *
soap_addressing_get_fault_to_address(struct SoapEnv *envelope)
{
  xmlNodePtr fault_to;

  fault_to = soap_addressing_get_fault_to(envelope);
  if (fault_to == NULL)
    return NULL;

  return soap_addressing_get_address(fault_to);
}

xmlNodePtr
soap_addressing_set_fault_to_address(struct SoapEnv *envelope, xmlURI *address)
{
   xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_fault_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_FAULT_TO, NULL);
  soap_addressing_set_address(ret, address);

  return ret;
}

xmlNodePtr
soap_addressing_get_to(struct SoapEnv *envelope)
{
  return _soap_addressing_get_child_element(envelope->header, WSA_TO);
}

xmlNodePtr
soap_addressing_set_to(struct SoapEnv *envelope, xmlNodePtr address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_TO, NULL);
  node = xmlCopyNode(address, 1);
  xmlUnlinkNode(node);
  xmlAddChild(ret, node);

  return ret;
}

xmlURI *
soap_addressing_get_to_address(struct SoapEnv *envelope)
{
  xmlNodePtr to;

  if (!(to = soap_addressing_get_to(envelope)))
    return NULL;

  return soap_addressing_get_address(to);
}

xmlChar *
soap_addressing_get_to_address_string(struct SoapEnv *envelope)
{
  xmlURI *uri;
  xmlChar *ret;
  
  if (!(uri = soap_addressing_get_to_address(envelope)))
    return NULL;

  ret = xmlSaveUri(uri);
  xmlFreeURI(uri);
  
  return ret;
}

xmlNodePtr
soap_addressing_set_to_address_string(struct SoapEnv *envelope, const char *to)
{
  xmlURI *uri;
  xmlNodePtr ret;

  if (!(uri = xmlParseURI(to)))
    return NULL;

  ret = soap_addressing_set_to_address(envelope, uri);
  xmlFreeURI(uri);

  return ret;
}

xmlNodePtr
soap_addressing_set_to_address(struct SoapEnv *envelope, xmlURI *address)
{
  xmlNodePtr ret;
  xmlNodePtr node;

  node = soap_addressing_get_to(envelope);
  if (node != NULL)
  {
    xmlUnlinkNode(node);
    xmlFreeNode(node);
  }

  ret = _soap_addressing_add_node(envelope->header, WSA_TO, NULL);
  soap_addressing_set_address(ret, address);

  return ret;
}

xmlURI *
soap_addressing_get_action(struct SoapEnv *envelope)
{
  xmlNodePtr action;

  action = _soap_addressing_get_child_element(envelope->header, WSA_ACTION);
  if (action == NULL)
    return NULL;

  return _soap_addressing_extract_uri(action);
}

xmlChar *
soap_addressing_get_action_string(struct SoapEnv *envelope)
{
  xmlURI *uri;
  xmlChar *ret;
  
  if (!(uri = soap_addressing_get_action(envelope)))
    return NULL;

  ret = xmlSaveUri(uri);
  xmlFreeURI(uri);

  return ret;
}

xmlNodePtr
soap_addressing_set_action(struct SoapEnv *envelope, xmlURI *action)
{
  xmlNodePtr node;

  node = _soap_addressing_get_child_element(envelope->header, WSA_ACTION);
  if (node == NULL)
    node = _soap_addressing_add_node(envelope->header, WSA_ACTION, NULL);

  return _soap_addressing_set_content_uri(node, action);
}

xmlNodePtr
soap_addressing_set_action_string(struct SoapEnv *envelope, const char *action)
{
  xmlURI *uri;
  xmlNodePtr ret;

  if (!(uri = xmlParseURI(action)))
    return NULL;

  ret = soap_addressing_set_action(envelope, uri);
  xmlFreeURI(uri);

  return ret;
}
