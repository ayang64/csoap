/******************************************************************
 *  $Id: soap-call.c,v 1.1 2004/01/30 16:39:34 snowdrop Exp $
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
#include <libcsoap/soap-call.h>
#include <libcsoap/soap-xml.h>
#include <libcsoap/soap-fault.h>
#include <nanohttp/nanohttp-client.h>
#include <stdarg.h>


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


/*--------------------------------- */
static 
xmlDocPtr _soap_call_build_result(hresponse_t *res);
/*--------------------------------- */


SoapCall* soap_call_new(const char *urn, const char *method)
{
  xmlDocPtr env;
  xmlNodePtr node;
  SoapCall *call;
  char buffer[1054];

  
  log_verbose3("URN = '%s'\nMethod = '%s'\n", urn, method);

  sprintf(buffer, _SOAP_MSG_TEMPLATE_, 
	  soap_env_ns, soap_env_enc, soap_xsi_ns, 
	  soap_xsd_ns, method, urn, method);

  env = xmlParseDoc(buffer);
  if (env == NULL) {
    log_error1("Can not create xml document!");
    return NULL;
  }

  node = xmlDocGetRootElement(env);
  if (node == NULL) {
    log_error1("xml document is empty!");
    return NULL;
  }
  
  call = (SoapCall*)malloc(sizeof(SoapCall));

  /* set root */
  call->root = node;

  /* set method root
     set call->cur (current node) to <method>.
     xpath: //Envelope/Body/
   */
  node = soap_xml_get_children(call->root);
  call->cur = soap_xml_get_children(node);

  return call;
}



xmlNodePtr
soap_call_add_param(SoapCall *call, const char *type, 
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
soap_call_add_paramf(SoapCall *call, const char *type,
		     const char *name, const char *format, ...)
{

  va_list ap;
  char buffer[1054];
  xmlNodePtr newnode;
  

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  return soap_call_add_param(call, type, name, buffer);
}

xmlNodePtr
soap_call_push_param(SoapCall *call, const char *type,
		     const char *name)
{

  xmlNodePtr node;

  node =  soap_call_add_param(call, type, name, "");

  if (node) {
    call->cur = node;
  }

  return node;
}


void
soap_call_pop_param(SoapCall *call)
{
  call->cur = call->cur->parent;
}


xmlDocPtr
soap_call_invoke(SoapCall *call, const char *url)
{
  /* Result document */
  xmlDocPtr doc;

  /* Buffer variables*/
  xmlBufferPtr buffer;
  char *content;

  /* Transport variables */
  httpc_conn_t *conn;
  hresponse_t *res;
  
  /* Create buffer */
  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, call->root->doc,call->root, 1 ,2);
  content = (char*)xmlBufferContent(buffer);
  
  /* Transport via HTTP */
  conn = httpc_new();
  res = httpc_post(conn, url, strlen(content), content);

  /* Free buffer */
  xmlBufferFree(buffer);

  /* Build result */
  doc = _soap_call_build_result(res);

  return doc;
}


static 
xmlDocPtr _soap_call_build_result(hresponse_t *res)
{
  xmlDocPtr doc;

  if (res == NULL)
    return soap_fault_build(Fault_Client, 
			    "Response is NULL","","");

  if (res->body)
    return soap_fault_build(Fault_Client, 
			    "Empty response from server!","","");


  doc = xmlParseDoc(res->body);
  if (doc == NULL) {
    return soap_fault_build(Fault_Client, 
			    "Response is not in XML format!","","");
  }

  return doc;
}

