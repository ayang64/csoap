/******************************************************************
 *  $Id: soap-server.c,v 1.1 2004/02/03 08:10:05 snowdrop Exp $
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
#include <libcsoap/soap-server.h>
#include <nanohttp/nanohttp-server.h>



typedef struct _SoapRouterNode
{
  char *context;
  SoapRouter *router;
  struct _SoapRouterNode *next;

}SoapRouterNode;

SoapRouterNode *head = NULL;
SoapRouterNode *tail = NULL;

static
SoapRouterNode *router_node_new(SoapRouter *router, 
				const char *context, 
				SoapRouterNode *next);

static
SoapRouter *router_find(const char *context);

/*---------------------------------*/
void soap_server_entry(httpd_conn_t *conn, hrequest_t *req);
static
void _soap_server_send_env(hsocket_t sock, SoapEnv* env);
static
void _soap_server_send_fault(httpd_conn_t *conn, hpair_t *header, 
			     const char* errmsg);
/*---------------------------------*/


int soap_server_init_args(int argc, char *argv[])
{
  return !httpd_init(argc, argv);
}


int soap_server_register_router(SoapRouter *router, const char* context)
{
  
  if (!httpd_register(context, soap_server_entry)) {
    return 0;
  }
  
  if (tail == NULL) {
    head = tail = router_node_new(router, context, NULL);
  } else {
    tail->next =  router_node_new(router, context, NULL);
    tail = tail->next;
  }
  
  return 1;
}


int soap_server_run()
{
  return httpd_run();
}


void soap_server_destroy()
{
  SoapRouterNode *node = head;
  SoapRouterNode *tmp;

  while (node != NULL) {
    tmp = node->next;
    soap_router_free(node->router);
    free(node->context);
    free(node);
    node = tmp;
  }
}


void soap_server_entry(httpd_conn_t *conn, hrequest_t *req)
{
  hpair_t *header;
  char *postdata;
  char buffer[1054];
  char urn[150];
  char method[150];
  long received;
  SoapEnv *env, *envres;
  SoapRouter *router;
  SoapService *service;

  if (strcmp(req->method, "POST")) {
    
    httpd_send_header(conn, 200, "OK", header);
    hsocket_send(conn->sock, "<html><head></head><body>");
    hsocket_send(conn->sock, "<h1>Sorry! </h1><hr>");
    hsocket_send(conn->sock, "I only speak with 'POST' method");
    hsocket_send(conn->sock, "</body></html>");
    return;
  }

  postdata = httpd_get_postdata(conn, req, &received, -1);

  header = hpairnode_new(HEADER_CONTENT_TYPE, "text/xml", NULL);


  if (postdata == NULL) {
    
    _soap_server_send_fault(conn, header,"Can not receive POST data!");

  } else {

    env = soap_env_new_from_buffer(postdata);

    if (env == NULL) {

      _soap_server_send_fault(conn, header,"Can not parse POST data!");
      
    } else {

      /*soap_xml_doc_print(env->root->doc);*/

      router = router_find(req->path);

      if ( router == NULL) {
	
	_soap_server_send_fault(conn, header, "Can not find router!");
      
      } else {

	if (!soap_env_find_urn(env, urn)) {
	  
	  _soap_server_send_fault(conn, header, "No URN found!");
	  return;
	} else {
	  log_verbose2("urn: '%s'", urn);
	}

	if (!soap_env_find_methodname(env, method)) {
	  
	  _soap_server_send_fault(conn, header, "No method found!");
	  return;
	}else {
	  log_verbose2("method: '%s'", method);
	}

	service = soap_router_find_service(router, urn, method);

	if (service == NULL) {

	  sprintf(buffer, "URN '%s' not found", urn);
	  _soap_server_send_fault(conn, header, buffer);
	  return;
	} else {

	  log_verbose2("func: %p", service->func);
	  envres = service->func(env);
	  log_verbose2("func returned: (%p)", envres);
	  if (envres == NULL) {

	    sprintf(buffer, "Service '%s' returned no envelope", urn);
	    _soap_server_send_fault(conn, header, buffer);
	    return;

	  } else {
	  
	    httpd_send_header(conn, 200, "OK", header);
	    _soap_server_send_env(conn->sock, envres);
	    /* free envres */
	  }

	}

      }   
    }
  }
}


static
void _soap_server_send_env(hsocket_t sock, SoapEnv* env)
{
  xmlBufferPtr buffer;
  if (env == NULL || env->root == NULL) return;

  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, env->root->doc, env->root, 1 ,1);
  hsocket_send(sock,  (const char*)xmlBufferContent(buffer));
  xmlBufferFree(buffer);  
  
}

static
void _soap_server_send_fault(httpd_conn_t *conn, hpair_t *header, 
			     const char* errmsg)
{
  SoapEnv *envres;
  httpd_send_header(conn, 500, "FAILED", header);
  envres = soap_env_new_with_fault(Fault_Server,
				   errmsg?errmsg:"General error",
				   "cSOAP_Server", NULL);
  _soap_server_send_env(conn->sock, envres);

}



static 
SoapRouterNode *router_node_new(SoapRouter *router, 
				const char *context,
				SoapRouterNode *next)
{
  SoapRouterNode *node;
  const char *noname = "/lost_find";

  node = (SoapRouterNode*)malloc(sizeof(SoapRouterNode));
  if (context) {
    node->context = (char*)malloc(strlen(context)+1);
    strcpy(node->context, context);
  } else {
    log_warn2("context is null. Using '%s'", noname);
    node->context = (char*)malloc(strlen(noname)+1);
    strcpy(node->context, noname);
  }

  node->router = router;
  node->next = next;

  return node;
}


static
SoapRouter *router_find(const char* context)
{
  SoapRouterNode *node = head;

  while (node != NULL) {
    if (!strcmp(node->context, context))
      return node->router;
    node = node->next;
  }
  
  return NULL;
}
