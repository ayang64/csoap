/******************************************************************
*  $Id: nanohttp-admin.c,v 1.3 2006/11/21 20:59:02 m0gg Exp $
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

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-server.h"
#include "nanohttp-admin.h"


static void
_httpd_admin_send_title(httpd_conn_t *conn, const char *title)
{
  httpd_send_header(conn, 200, "OK");
  http_output_stream_write_string(conn->out,
   "<html><head><style>");
  http_output_stream_write_string(conn->out,
   ".logo {"
   " color: #005177;"
   " background-color: transparent;"
   " font-family: Calligraphic, arial, sans-serif;"
   " font-size: 36px;"
   "}");
  http_output_stream_write_string(conn->out,
   "</style></head><body><span class=\"logo\">nhttpd</span> ");
  http_output_stream_write_string(conn->out, title);
  http_output_stream_write_string(conn->out, "<hr />");

  return;
}


static void
_httpd_admin_list_services(httpd_conn_t *conn)
{
  char buffer[1024];
  hservice_t *node;

  _httpd_admin_send_title(conn, "Available services");

  http_output_stream_write_string(conn->out, "<ul>");
  for (node = httpd_get_services(); node; node = node->next)
  {
    sprintf(buffer, "<li><a href=\"%s\">%s</a> <a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a></li>", node->ctx, node->ctx, node->ctx);
    http_output_stream_write_string(conn->out, buffer);
  }
  http_output_stream_write_string(conn->out, "</ul>");
  
  http_output_stream_write_string(conn->out, "</body></html>");

  return;
}


static void
_httpd_admin_list_statistics(httpd_conn_t *conn, const char *service_name)
{
  char buffer[1024];
  hservice_t *service;
  
  sprintf(buffer, "Listing statistics for service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!(service = httpd_find_service(service_name)))
  {
    http_output_stream_write_string(conn->out, "service not found!");
    http_output_stream_write_string(conn->out, "</body></html>");
    return;
  }

  pthread_rwlock_rdlock(&(service->statistics->lock));
  sprintf(buffer, "<ul>"
                    "<li>Requests served: %lu</li>"
                    "<li>Bytes read: %lu</li>"
                    "<li>Bytes sent: %lu</li>"
                    "<li>Time used: %li.%li sec</li>"
                  "<ul>",
                  service->statistics->requests,
                  service->statistics->bytes_received,
                  service->statistics->bytes_transmitted,
		  service->statistics->time.tv_sec,
		  service->statistics->time.tv_usec);
  pthread_rwlock_unlock(&(service->statistics->lock));

  http_output_stream_write_string(conn->out, buffer);

  http_output_stream_write_string(conn->out, "</body></html>");

  return;
}


static void
_httpd_admin_handle_get(httpd_conn_t * conn, hrequest_t * req)
{
  char *param;

  if ((param = hpairnode_get_ignore_case(req->query, NHTTPD_ADMIN_QUERY_SERVICES)))
  {
    _httpd_admin_list_services(conn);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, NHTTPD_ADMIN_QUERY_STATISTICS)))
  {
    _httpd_admin_list_statistics(conn, param);
  }
  else
  {
    _httpd_admin_send_title(conn, "Welcome to the admin site");

    http_output_stream_write_string(conn->out, "<ul>");
    http_output_stream_write_string(conn->out,
     "<li><a href=\"?" NHTTPD_ADMIN_QUERY_SERVICES "\">Services</a></li>");
    http_output_stream_write_string(conn->out, "</ul>");

    http_output_stream_write_string(conn->out, "</body></html>");
  }

  return;
}


static void
_httpd_admin_entry(httpd_conn_t * conn, hrequest_t * req)
{
  if (req->method == HTTP_REQUEST_GET)
  {
    _httpd_admin_handle_get(conn, req);
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
                      "<div>POST Service is not implemented now. Use your browser.</div>"
                  "</body>"
              "</html>");
  }
  return;
}


herror_t
httpd_admin_init_args(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++) {

    if (!strcmp(argv[i], NHTTPD_ARG_ENABLE_ADMIN)) {

      httpd_register("/nhttp", _httpd_admin_entry);
      break;
    }
  }

  return H_OK;
}
