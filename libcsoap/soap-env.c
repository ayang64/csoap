/******************************************************************
*  $Id: soap-env.c,v 1.28 2007/11/03 22:40:09 m0gg Exp $
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <nanohttp/nanohttp-error.h>

#include "soap-logging.h"
#include "soap-xml.h"
#include "soap-fault.h"
#include "soap-env.h"

/*
Parameters:
1- soap_env_ns
2- soap_env_enc
3- xsi_ns
4- xsd_ns
3- method name
4- uri
5- method name(again)
*/
#define _SOAP_MSG_TEMPLATE_ \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"%s\" SOAP-ENV:encodingStyle=\"%s\"" \
	" xmlns:xsi=\"%s\"" \
	" xmlns:xsd=\"%s\">" \
         "<SOAP-ENV:Header />" \
	 "<SOAP-ENV:Body>"\
	  "<m:%s xmlns:m=\"%s\">"\
	  "</m:%s>" \
	 "</SOAP-ENV:Body>"\
	"</SOAP-ENV:Envelope>"


/*
Parameters:
1- soap_env_ns
2- soap_env_enc
3- xsi_ns
4- xsd_ns
3- method name
4- uri
5- method name(again)
*/
#define _SOAP_MSG_TEMPLATE_EMPTY_TARGET_ \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"%s\" SOAP-ENV:encodingStyle=\"%s\"" \
	" xmlns:xsi=\"%s\"" \
	" xmlns:xsd=\"%s\">" \
         "<SOAP-ENV:Header />" \
	 "<SOAP-ENV:Body>"\
	  "<%s xmlns=\"%s\">"\
	  "</%s>" \
	 "</SOAP-ENV:Body>"\
	"</SOAP-ENV:Envelope>"


/* ---------------------------------------------------------------------------- */
/*     XML Serializers (implemented at the and of this document)              */
/* ---------------------------------------------------------------------------- */
struct XmlNodeHolder
{
  xmlNodePtr node;
};

static void
xmlbuilder_start_element(const xmlChar * element_name, int attr_count, xmlChar ** keys, xmlChar ** values, void *userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  holder->node = xmlNewChild(parent, NULL, element_name, NULL);

  return;
}

static void
xmlbuilder_characters(const xmlChar * element_name, const xmlChar * chars, void *userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  xmlNewTextChild(parent, NULL, element_name, chars);

  return;
}

static void
xmlbuilder_end_element(const xmlChar * element_name, void *userData)
{

  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  holder->node = parent->parent;

  return;
}

herror_t
soap_env_new_from_doc(xmlDocPtr doc, struct SoapEnv ** out)
{
  xmlNodePtr root;
  struct SoapEnv *env;

  if (doc == NULL)
  {
    log_error("Cannot create XML document!");
    return herror_new("soap_env_new_from_doc",
                      GENERAL_INVALID_PARAM,
                      "XML Document (xmlDocPtr) is NULL");
  }

  if (!(root = xmlDocGetRootElement(doc)))
  {
    log_error("XML document is empty!");
    return herror_new("soap_env_new_from_doc",
                      XML_ERROR_EMPTY_DOCUMENT, "XML Document is empty!");
  }

  if (!(env = (struct SoapEnv *) malloc(sizeof(struct SoapEnv))))
  {
    log_error("malloc failed (%s)", strerror(errno));
    return herror_new("soap_env_from_doc", GENERAL_INVALID_PARAM, "malloc failed (%s)", strerror(errno));
  }

  env->root = root;
  env->header = soap_env_get_header(env);
  env->body = soap_env_get_body(env);
  env->cur = soap_env_get_method(env);

  *out = env;

  return H_OK;
}

herror_t
soap_env_new_from_buffer(const char *buffer, struct SoapEnv **out)
{
  xmlDocPtr doc;
  herror_t err;

  if (buffer == NULL)
    return herror_new("soap_env_new_from_buffer",
                      GENERAL_INVALID_PARAM, "buffer (first param) is NULL");

  if (!(doc = xmlParseDoc(BAD_CAST buffer)))
    return herror_new("soap_env_new_from_buffer",
                      XML_ERROR_PARSE, "Cannot parse XML");

  if ((err = soap_env_new_from_doc(doc, out)) != H_OK)
  {
    xmlFreeDoc(doc);
  }
  return err;
}


herror_t
soap_env_new_with_fault(int faultcode, const char *faultstring, const char *faultactor, const char *detail, struct SoapEnv **out)
{
  xmlDocPtr doc;
  herror_t err;

  if (!(doc = soap_fault_build(faultcode, faultstring, faultactor, detail)))
    return herror_new("soap_env_new_with_fault", XML_ERROR_PARSE, "Cannot parse fault XML");

  if ((err = soap_env_new_from_doc(doc, out)) != H_OK)
    xmlFreeDoc(doc);

  return err;
}


herror_t
soap_env_new_with_response(struct SoapEnv * request, struct SoapEnv ** out)
{
  const char *method;
  char *res_method;
  herror_t ret;
  const char *urn;

  if (request == NULL)
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM, "request (first param) is NULL");
  }

  if (request->root == NULL)
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM,
                      "request (first param) has no XML structure");
  }

  if (!(method = soap_env_find_methodname(request)))
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM,
                      "Method name not found in request");
  }

  if (!(urn = soap_env_find_urn(request)))
  {

    /* here we have no chance to find out the namespace */
    /* try to continue without namespace (urn) */
    urn = "";
  }

  if (!(res_method = (char *)malloc(strlen(method)+9)))
    return herror_new("soap_env_new_with_response", GENERAL_INVALID_PARAM, "malloc failed");

  sprintf(res_method, "%sResponse", method);

  ret = soap_env_new_with_method(urn, res_method, out);

  free(res_method);

  return ret;
}


herror_t
soap_env_new_with_method(const char *urn, const char *method, struct SoapEnv ** out)
{
  xmlDocPtr env;
  xmlChar buffer[1054];

  log_verbose("URN = '%s'", urn);
  log_verbose("Method = '%s'", method);

  if (!strcmp(urn, ""))
  {
    xmlStrPrintf(buffer, 1054, BAD_CAST _SOAP_MSG_TEMPLATE_EMPTY_TARGET_,
                 soap_env_ns, soap_env_enc, soap_xsi_ns,
                 soap_xsd_ns, BAD_CAST method, BAD_CAST urn, BAD_CAST method);
  }
  else
  {
    xmlStrPrintf(buffer, 1054, BAD_CAST _SOAP_MSG_TEMPLATE_,
                 soap_env_ns, soap_env_enc, soap_xsi_ns,
                 soap_xsd_ns, BAD_CAST method, BAD_CAST urn, BAD_CAST method);
  }

  if (!(env = xmlParseDoc(buffer)))
    return herror_new("soap_env_new_with_method",
                      XML_ERROR_PARSE, "Can not parse XML");

  return soap_env_new_from_doc(env, out);
}


xmlNodePtr
soap_env_add_item(struct SoapEnv * call, const char *type, const char *name, const char *value)
{
  xmlNodePtr newNode;

  newNode = xmlNewTextChild(call->cur, NULL, BAD_CAST name, BAD_CAST value);

  if (newNode == NULL)
  {
    log_error("Can not create new XML node");
    return NULL;
  }

  if (type)
  {
    if (!xmlNewProp(newNode, BAD_CAST "xsi:type", BAD_CAST type))
    {
      log_error("Can not create new XML attribute");
      return NULL;
    }
  }

  return newNode;
}


xmlNodePtr
soap_env_add_itemf(struct SoapEnv * call, const char *type, const char *name, const char *format, ...)
{

  va_list ap;
  char buffer[1054];

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  return soap_env_add_item(call, type, name, buffer);
}


xmlNodePtr
soap_env_add_attachment(struct SoapEnv * call, const char *name, const char *href)
{
  xmlNodePtr newNode;

  newNode = xmlNewTextChild(call->cur, NULL, BAD_CAST name, BAD_CAST "");

  if (newNode == NULL)
  {
    log_error("Can not create new xml node");
    return NULL;
  }

  if (href)
  {
    if (!xmlNewProp(newNode, BAD_CAST "href", BAD_CAST href))
    {
      log_error("Can not create new xml attribute");
      return NULL;
    }
  }

  return newNode;
}


void
soap_env_add_custom(struct SoapEnv * call, void *obj, XmlSerializerCallback cb, const char *type, const char *name)
{
  struct XmlNodeHolder holder;

  holder.node = soap_env_get_method(call);

  cb(obj, BAD_CAST name,
     xmlbuilder_start_element,
     xmlbuilder_characters, xmlbuilder_end_element, &holder);

  return;
}


xmlNodePtr
soap_env_push_item(struct SoapEnv * call, const char *type, const char *name)
{

  xmlNodePtr node;

  if ((node = soap_env_add_item(call, type, name, "")))
    call->cur = node;

  return node;
}


void
soap_env_pop_item(struct SoapEnv * call)
{
  call->cur = call->cur->parent;

  return;
}


void
soap_env_free(struct SoapEnv * env)
{
  if (env)
  {
    if (env->root)
    {
      xmlFreeDoc(env->root->doc);
    }
    free(env);
  }

  return;
}

xmlNodePtr
soap_env_get_body(struct SoapEnv * env)
{
  xmlNodePtr node;

  if (env == NULL)
  {
    log_error("SOAP envelope is NULL");
    return NULL;
  }

  if (env->root == NULL)
  {
    log_error("SOAP envelope contains no XML");
    return NULL;
  }

  for (node = soap_xml_get_children(env->root); node; node = soap_xml_get_next_element(node))
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Body")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns))
      return node;
  }

  log_error("Body tag not found!");
  return NULL;
}


xmlNodePtr
soap_env_get_header(struct SoapEnv *env)
{
  xmlNodePtr node;

  if (!env)
  {
    log_error("SoapEnv is NULL");
    return NULL;
  }

  if (!env->root)
  {
    log_error("SoapEnv contains no document");
    return NULL;
  }

  for (node = soap_xml_get_children(env->root); node; node = soap_xml_get_next_element(node))
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Header")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns))
      return node;
  }

  return NULL;
}


xmlNodePtr
soap_env_get_fault(struct SoapEnv * env)
{
  xmlNodePtr node;

  if (!(node = soap_env_get_body(env)))
  {
    return NULL;
  }

  while (node != NULL)
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Fault"))
      return node;
    node = soap_xml_get_next_element(node);
  }

/*  log_warn("Node Fault tag found!");*/
  return NULL;
}


xmlNodePtr
soap_env_get_method(struct SoapEnv * env)
{
  xmlNodePtr body;

  if (!(body = soap_env_get_body(env)))
  {
    log_verbose("SoapEnv contains no Body element");
    return NULL;
  }

  /* The method element has to be the first child element */
  return soap_xml_get_children(body);
}


const char *
soap_env_find_urn(struct SoapEnv * env)
{
  xmlNsPtr ns;
  xmlNodePtr body, node;

  if (!(body = soap_env_get_body(env)))
  {
    log_verbose("body is NULL");
    return NULL;
  }

  /* node is the first child */
  if (!(node = soap_xml_get_children(body)))
  {
    log_error("No namespace found");
    return NULL;
  }

  /* if (node->ns && node->ns->prefix) MRC 1/25/2006 */
  if (node->ns)
  {
    ns = xmlSearchNs(body->doc, node, node->ns->prefix);
    if (ns != NULL)
    {
      return ((char *) ns->href); /* namespace found! */
    }
  }
  else
  {
    static const char *empty = "";
    log_warn("No namespace found");
    return(empty);
  }

  return NULL;
}


const char *
soap_env_find_methodname(struct SoapEnv * env)
{
  xmlNodePtr body, node;

  if (!(body = soap_env_get_body(env)))
    return NULL;

  /* node is the first child */
  if (!(node = soap_xml_get_children(body)))
  {
    log_error("No method found");
    return NULL;
  }

  if (node->name == NULL)
  {
    log_error("No methodname found");
    return NULL;
  }

  return ((char *) node->name);
}
