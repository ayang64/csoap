/** @file soap-admin.c SOAP administrator application */
/******************************************************************
*  $Id: soap-admin.c,v 1.12 2007/11/03 22:40:09 m0gg Exp $
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

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-server.h>
#include <nanohttp/nanohttp-admin.h>

#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-server.h"
#include "soap-admin.h"

static void
_soap_admin_send_title(httpd_conn_t *conn, const char *title)
{
  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);

  http_output_stream_write_string(conn->out,
   "<html>"
     "<head>");

  http_output_stream_write_string(conn->out,
       "<style>"
         ".logo {"
         " color: #005177;"
         " background-color: transparent;"
         " font-family: Calligraphic, arial, sans-serif;"
         " font-size: 36px;"
         "}"
       "</style>"
     "</head>"
     "<body>"
       "<span class=\"logo\">csoap</span> ");

  http_output_stream_write_string(conn->out, title);
  http_output_stream_write_string(conn->out, "<hr />");
}

static inline void
_soap_admin_send_footer(httpd_conn_t *conn)
{
  http_output_stream_write_string(conn->out,
        "<hr />"
	"<a href=\"" CSOAP_ADMIN_CONTEXT "\">Admin page</a> "
        "<a href=\"http://csoap.sf.net/\">cSOAP Home</a>"
      "</body>"
    "</html>");
}

static void
_soap_admin_list_routers(httpd_conn_t *conn)
{
  SoapRouterNode *node;
  char buffer[1024];

  _soap_admin_send_title(conn, "Available routers");

  http_output_stream_write_string(conn->out, "<ul>");
  for (node = soap_server_get_routers(); node; node = node->next)
  {
    sprintf(buffer,
      "<li>"
        "<a href=\"?" CSOAP_ADMIN_QUERY_ROUTER "=%s\">%s</a> - "
        "<a href=\"%s\">[Service Description]</a> - "
        "<a href=\"../nhttp?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a>"
      "</li>",
      node->context, node->context, node->context, node->context);
    http_output_stream_write_string(conn->out, buffer);
  }
  http_output_stream_write_string(conn->out, "</ul>");
  
  _soap_admin_send_footer(conn);
}

static void
_soap_admin_list_services(httpd_conn_t *conn, const char *routername)
{
  struct SoapRouter *router;
  SoapServiceNode *node;
  char buffer[1024];
  
  sprintf(buffer, "Listing Services for Router <b>%s</b>", routername);
  _soap_admin_send_title(conn, buffer);

  router = soap_server_find_router(routername);
  if (!router)
  {
    http_output_stream_write_string(conn->out, "Router not found!");
    _soap_admin_send_footer(conn);
    return;
  }

  node = router->service_head;

  http_output_stream_write_string(conn->out, "<ul>");
  for (node = router->service_head; node; node = node->next)
  {
    switch (node->service->status)
    {
      case CSOAP_SERVICE_DOWN:
        sprintf(buffer,
          "<li>"
            "%s (%s) <a href=\"?" CSOAP_ADMIN_QUERY_ACTIVATE "=%s&amp;" CSOAP_ADMIN_URN "=%s\">[Activate]</a>"
          "</li>",
          node->service->urn,
          node->service->method,
	  node->service->method,
	  node->service->urn);
        break;
      case CSOAP_SERVICE_UP:
      default:
        sprintf(buffer,
        "<li>"
          "%s (%s) <a href=\"?" CSOAP_ADMIN_QUERY_PASSIVATE "=%s&amp;" CSOAP_ADMIN_URN "=%s\">[Passivate]</a>"
        "</li>",
        node->service->urn,
        node->service->method,
	node->service->method,
	node->service->urn);
	break;
    }
    http_output_stream_write_string(conn->out, buffer);
  }
  http_output_stream_write_string(conn->out, "</ul>");

  _soap_admin_send_footer(conn);
}

static void
_soap_admin_handle_get(httpd_conn_t * conn, struct hrequest_t * req)
{
  char *param;

  if ((param = hpairnode_get_ignore_case(req->query, CSOAP_ADMIN_QUERY_ROUTERS)))
  {
    _soap_admin_list_routers(conn);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, CSOAP_ADMIN_QUERY_ROUTER)))
  {
    _soap_admin_list_services(conn, param);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, CSOAP_ADMIN_QUERY_ACTIVATE)))
  {
    _soap_admin_send_title(conn, "Not implemented");
    _soap_admin_send_footer(conn);
    // _soap_admin_activate_service(conn, param);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, CSOAP_ADMIN_QUERY_PASSIVATE)))
  {
    _soap_admin_send_title(conn, "Not implemented");
    _soap_admin_send_footer(conn);
    // _soap_admin_passivate_service(conn, param);
  }
  else
  {
    _soap_admin_send_title(conn, "Welcome to the admin site");

    http_output_stream_write_string(conn->out,
      "<ul>"
        "<li><a href=\"?" CSOAP_ADMIN_QUERY_ROUTERS "\">Routers</a></li>"
	"<li><a href=\"../inspection.wsil\">inspection.wsil</a> (try: -CSOAPwsil)</li>"
        "<li><a href=\"" NHTTPD_ADMIN_CONTEXT "\">nanoHTTP</a> (try: -NHTTPDadmin)</li>"
      "</ul>");

    _soap_admin_send_footer(conn);
  }
}

static void
_soap_admin_entry(httpd_conn_t * conn, struct hrequest_t * req)
{
  if (req->method == HTTP_REQUEST_GET)
  {
    _soap_admin_handle_get(conn, req);
  }
  else
  {
    httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    http_output_stream_write_string(conn->out,
      "<html>"
        "<head>"
        "</head>"
        "<body>"
          "<h1>Sorry!</h1>"
          "<hr />"
          "<div>Only GET method is implemented now. Use your browser.</div>"
          "</body>"
        "</html>");
  }
}

herror_t
soap_admin_init_args(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++) {

    if (!strcmp(argv[i], CSOAP_ENABLE_ADMIN)) {

      httpd_register(CSOAP_ADMIN_CONTEXT, _soap_admin_entry);
      break;
    }
  }
  return H_OK;
}
