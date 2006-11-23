/******************************************************************
*  $Id: soap-wsil.c,v 1.1 2006/11/23 13:20:46 m0gg Exp $
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

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-server.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-server.h"
#include "soap-wsil.h"

static void
_soap_wsil_list_services(httpd_conn_t *conn, struct SoapRouter *router)
{
  SoapServiceNode *node;

  for (node=router->service_head; node; node=node->next)
  {
    http_output_stream_write_string(conn->out,
      "<service>"
        "<description "
          "referencedNamespace=\"http://schemas.xmlsoap.org/wsdl/\" "
	  "location=\"");
    http_output_stream_write_string(conn->out, soap_transport_get_name());
    http_output_stream_write_string(conn->out, node->service->method);
    http_output_stream_write_string(conn->out,
          "\" />"
      "</service>\n");
  }
  return;
}

static void
_soap_wsil_list_routers(httpd_conn_t *conn)
{
  SoapRouterNode *node;

  for (node=soap_server_get_routers(); node; node=node->next)
    _soap_wsil_list_services(conn, node->router);

  return;
}

static void
_soap_wsil_handle_get(httpd_conn_t * conn, struct hrequest_t * req)
{
  httpd_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");
  httpd_send_header(conn, 200, "OK");

  http_output_stream_write_string(conn->out,
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    "<wsil:inspection xmlns:wsil=\"http://schemas.xmlsoap.org/ws/2001/10/inspection/\">"
  );

  _soap_wsil_list_routers(conn);

  http_output_stream_write_string(conn->out,
    "</wsil:inspection>\n");

  return;
}

static void
_soap_wsil_entry(httpd_conn_t * conn, struct hrequest_t * req)
{
  if (req->method == HTTP_REQUEST_GET)
  {
    _soap_wsil_handle_get(conn, req);
  }
  else
  {
    httpd_send_header(conn, 200, "OK");
    http_output_stream_write_string(conn->out,
      "<html>"
        "<head>"
        "</head>"
        "<body>"
          "<h1>Sorry!</h1>"
          "<hr />"
          "<div>POST Service is not implemented now. Use your browser</div>"
        "</body>"
      "</html>");
  }
  return;
}

herror_t
soap_wsil_init_args(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++) {

    if (!strcmp(argv[i], CSOAP_ENABLE_WSIL)) {

      httpd_register("/inspection.wsil", _soap_wsil_entry);
      break;
    }
  }

  return H_OK;
}
