/******************************************************************
 *  $Id: soap-env.c,v 1.3 2004/06/08 12:54:09 snowdrop Exp $
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
#include <libcsoap/soap-env.h>
#include <stdarg.h>
#include <string.h>

static char *soap_env_ns = "http://schemas.xmlsoap.org/soap/envelope/";
static char *soap_env_enc = "http://schemas.xmlsoap.org/soap/encoding/";
static char *soap_xsi_ns = "http://www.w3.org/1999/XMLSchema-instance";
static char *soap_xsd_ns = "http://www.w3.org/1999/XMLSchema";

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
  "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"%s\" SOAP-ENV:encoding=\"%s\"" \
  " xmlns:xsi=\"%s\"" \
  " xmlns:xsd=\"%s\">" \
  " <SOAP-ENV:Body>"\
  "  <m:%s xmlns:m=\"%s\">"\
  "  </m:%s>" \
  " </SOAP-ENV:Body>"\
  "</SOAP-ENV:Envelope>"


/* ---------------------------------------------------------------------------- */
/*     XML Serializers (implemented at the and of this document)              */
/* ---------------------------------------------------------------------------- */
struct XmlNodeHolder { xmlNodePtr node; };

static
void xmlbuilder_start_element(const char* element_name, int attr_count, 
  char **keys, char **values, void* userData);

static
void xmlbuilder_characters(const char* element_name, 
  const char* chars, void* userData);

static
void xmlbuilder_end_element(const char* element_name, void* userData);

/* ---------------------------------------------------------------------------- */

SoapEnv *soap_env_new_with_fault(fault_code_t faultcode, 
				 const char *faultstring,
				 const char *faultactor,
				 const char *detail)
{
  xmlDocPtr doc;
  doc = soap_fault_build(faultcode, faultstring,
			 faultactor, detail);
  if (doc == NULL) return NULL;
  return soap_env_new_from_doc(doc);
}


SoapEnv *soap_env_new_with_response(SoapEnv* request)
{
  char urn[100];
  char methodname[150];
  char methodname2[150];

  if (request == NULL) {
    log_error1("request object is NULL");
    return NULL;
  }

  if (request->root == NULL) {
    log_error1("request has no xml");
    return NULL;
  }

  if (!soap_env_find_methodname(request, methodname)) {
    return NULL;
  }

  if (!soap_env_find_urn(request, urn)) {
    
    /* here we have no chance to find out the namespace */
    /* try to continue without namespace (urn) */
    urn[0] = '\0';
  } 

  sprintf(methodname2, "%sResponse", methodname);
  return soap_env_new_with_method(urn, methodname2);
}


SoapEnv *soap_env_new_with_method(const char *urn, const char *method)
{
  xmlDocPtr env;
  SoapEnv *call;
  char buffer[1054];

  
  log_verbose2("URN = '%s'", urn);
  log_verbose2("Method = '%s'",method);

  sprintf(buffer, _SOAP_MSG_TEMPLATE_, 
	  soap_env_ns, soap_env_enc, soap_xsi_ns, 
	  soap_xsd_ns, method, urn, method);

  env = xmlParseDoc(buffer);
  call = soap_env_new_from_doc(env);

  return call;
}



xmlNodePtr
soap_env_add_item(SoapEnv *call, const char *type, 
		  const char *name, const char *value)
{

  xmlNodePtr newnode;

  newnode = xmlNewTextChild(call->cur, NULL, name, value);

  if (newnode == NULL) {
    log_error1("Can not create new xml node");
    return NULL;
  }

  if (type) {
    if (!xmlNewProp(newnode, "xsi:type", type)) {
      log_error1("Can not create new xml attribute");
      return NULL;
    }
  }
  
  return newnode;  
}


xmlNodePtr
soap_env_add_itemf(SoapEnv *call, const char *type,
		    const char *name, const char *format, ...)
{

  va_list ap;
  char buffer[1054];
  

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  return soap_env_add_item(call, type, name, buffer);
}


void
soap_env_add_custom(SoapEnv *call, void *obj, XmlSerializerCallback cb, 
    const char *type, const char *name)
{
  struct XmlNodeHolder holder;

  holder.node = soap_env_get_method(call);

  cb(obj, name, 
    xmlbuilder_start_element,
    xmlbuilder_characters,
    xmlbuilder_end_element, &holder);

}


xmlNodePtr
soap_env_push_item(SoapEnv *call, const char *type,
		   const char *name)
{

  xmlNodePtr node;

  node =  soap_env_add_item(call, type, name, "");

  if (node) {
    call->cur = node;
  }

  return node;
}


void
soap_env_pop_item(SoapEnv *call)
{
  call->cur = call->cur->parent;
}

void soap_env_free(SoapEnv *env)
{
  if (env) {
    if (env->root) {
      xmlFreeDoc(env->root->doc);
    }
    free(env);
  }
}


SoapEnv *soap_env_new_from_doc(xmlDocPtr doc)
{
  SoapEnv *env;
  xmlNodePtr node;

  if (doc == NULL) {
    log_error1("Can not create xml document!");
    return NULL;
  }

  node = xmlDocGetRootElement(doc);
  if (node == NULL) {
    log_error1("xml document is empty!");
    return NULL;
  }
  
  env = (SoapEnv*)malloc(sizeof(SoapEnv));

  /* set root */
  env->root = node;

  /* set method root
     set call->cur (current node) to <method>.
     xpath: //Envelope/Body/
   */
  node = soap_xml_get_children(env->root);
  env->cur = soap_xml_get_children(node);

  return env;
}




SoapEnv *soap_env_new_from_buffer(const char* buffer)
{
  xmlDocPtr doc;
  SoapEnv *env;

  if (buffer == NULL) return NULL;

  doc = xmlParseDoc((xmlChar*)buffer);
  if (doc == NULL) return NULL;

  env = soap_env_new_from_doc(doc);
  if (env == NULL) {
    xmlFreeDoc(doc);
    return NULL;
  }

  return env;
}


xmlNodePtr
soap_env_get_body(SoapEnv* env)
{
  xmlNodePtr node;

  if (env == NULL) {
    log_error1("env object is NULL");
    return NULL;
  }

  if (env->root == NULL) {
    log_error1("env has no xml");
    return NULL;
  }

  node = soap_xml_get_children(env->root);

  while (node != NULL) {
    if (!strcmp(node->name, "Body"))
      return node;
    node = soap_xml_get_next(node);
  }

  log_error1("Node Body tag found!");
  return NULL;
}


xmlNodePtr
soap_env_get_method(SoapEnv* env)
{
  
  xmlNodePtr body;

  body = soap_env_get_body(env);
  if (body == NULL) {
    log_verbose1("body is NULL");
    return NULL;
  }

 /* mehtod is the first child */
  return soap_xml_get_children(body);
  
}


xmlNodePtr
_soap_env_get_body(SoapEnv* env)
{
  xmlNodePtr  body;
  xmlNodeSetPtr nodeset; 
  xmlXPathObjectPtr xpathobj;

  if (env == NULL) {
    log_error1("env object is NULL");
    return NULL;
  }

  if (env->root == NULL) {
    log_error1("env has no xml");
    return NULL;
  }

  /* 
     find <Body> tag find out namespace
     xpath: //Envelope/Body/
   */
  xpathobj = soap_xpath_eval(env->root->doc, "//Envelope/Body");

  if (!xpathobj) {
    log_error1("No Body (xpathobj)!");
    return NULL;
  }

  nodeset = xpathobj->nodesetval;
  if (!nodeset) {
    log_error1("No Body (nodeset)!");
    xmlXPathFreeObject(xpathobj);
    return NULL;
  }

  if (nodeset->nodeNr < 1) {
    log_error1("No Body (nodeNr)!");
    xmlXPathFreeNodeSet	(nodeset);
    xmlXPathFreeObject(xpathobj);
    return NULL;
  }

  body = nodeset->nodeTab[0]; /* body is <Body> */
  xmlXPathFreeNodeSet	(nodeset);
  xmlXPathFreeObject(xpathobj);
  return body;

}


int soap_env_find_urn(SoapEnv *env, char *urn)
{
  xmlNsPtr ns;
  xmlNodePtr body, node;

  body = soap_env_get_body(env);
  if (body == NULL) {
    log_verbose1("body is NULL");
    return 0;
  }

 /* node is the first child */
  node = soap_xml_get_children(body);
  
  if (node == NULL) {
    log_error1("No namespace found");
    return 0;
  } 

  if (node->ns && node->ns->prefix) {
    ns = xmlSearchNs(body->doc, node, node->ns->prefix);
    if (ns != NULL) {
      strcpy(urn, (char*)ns->href);
      return 1; /* namesapce found! */
    }
  }

  log_error1("No namespace found. Returning 0");
  return 0;
}


int soap_env_find_methodname(SoapEnv *env, char *method)
{
  xmlNodePtr body, node;

  body = soap_env_get_body(env);
  if (body == NULL) return 0;

  node = soap_xml_get_children(body); /* node is the first child */
  
  if (node == NULL) {
    log_error1("No method found");
    return 0;
  } 

  if (node->name == NULL) {
    log_error1("No methodname found");
    return 0;

  }

  strcpy(method, node->name);
  
  return 1;
}



/* ------------------------------------------------------------------ */
/*        XML serializers                                            */
/* ------------------------------------------------------------------ */


static
void xmlbuilder_start_element(const char* element_name, int attr_count, char **keys, char **values, void* userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder*)userData;
  xmlNodePtr parent = NULL;
  
  if (holder == NULL) return;
  parent = holder->node;
  if (parent == NULL) return;

  holder->node = xmlNewChild(parent, NULL, element_name, NULL);

}

static
void xmlbuilder_characters(const char* element_name, const char* chars, void* userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder*)userData;
  xmlNodePtr parent = NULL;
  
  if (holder == NULL) return;
  parent = holder->node;
  if (parent == NULL) return;

  xmlNewTextChild(parent, NULL, element_name, chars);
}

static
void xmlbuilder_end_element(const char* element_name, void* userData)
{

  struct XmlNodeHolder *holder = (struct XmlNodeHolder*)userData;
  xmlNodePtr parent = NULL;
  
  if (holder == NULL) return;
  parent = holder->node;
  if (parent == NULL) return;

  holder->node = parent->parent;
}

