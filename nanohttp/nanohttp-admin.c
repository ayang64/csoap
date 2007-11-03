/** @file nanohttp-admin.c Administrator application */
/******************************************************************
*  $Id: nanohttp-admin.c,v 1.13 2007/11/03 22:40:10 m0gg Exp $
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

#include "nanohttp-error.h"
#include "nanohttp-logging.h"
#include "nanohttp-common.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-server.h"

#include "nanohttp-admin.h"

static void
_httpd_admin_send_title(httpd_conn_t *conn, const char *title)
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
       "</style>");

  http_output_stream_write_string(conn->out,
     "</head>"
     "<body>"
       "<span class=\"logo\">nhttpd</span> ");
  http_output_stream_write_string(conn->out, title);
  http_output_stream_write_string(conn->out, "<hr />");
}

static inline void
_httpd_admin_send_footer(httpd_conn_t *conn)
{
  http_output_stream_write_string(conn->out,
        "<hr />"
        "<a href=\"" NHTTPD_ADMIN_CONTEXT "\">Admin page</a> "
	"<a href=\"http://csoap.sf.net/\">cSOAP Home</a>"
      "</body>"
    "</html>");
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
    switch (node->status)
    {
      case NHTTPD_SERVICE_DOWN:
        sprintf(buffer,
          "<li>"
            "<a href=\"%s\">%s</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_ACTIVATE_SERVICE "=%s\">[Activate]</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a>"
          "</li>",
          node->context, node->context, node->context, node->context);
        break;
      case NHTTPD_SERVICE_UP:
      default:
        sprintf(buffer,
          "<li>"
            "<a href=\"%s\">%s</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_PASSIVATE_SERVICE "=%s\">[Passivate]</a> "
            "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "=%s\">[Statistics]</a> "
          "</li>",
          node->context, node->context, node->context, node->context);
        break;
    }
    http_output_stream_write_string(conn->out, buffer);
  }
  http_output_stream_write_string(conn->out, "</ul>");
  
  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_list_statistics(httpd_conn_t *conn, const char *service_name)
{
  char buffer[1024];
  hservice_t *service;
  
  log_verbose("Client requested statistics for \"%s\"", service_name);

  sprintf(buffer, "Listing statistics for service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!service_name || !(service = httpd_find_service(service_name)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");
    _httpd_admin_send_footer(conn);
    return;
  }

  pthread_rwlock_rdlock(&(service->statistics->lock));
  sprintf(buffer, "<ul>"
                    "<li>Requests served: %lu</li>"
                    "<li>Bytes read: %lu</li>"
                    "<li>Bytes sent: %lu</li>"
                    "<li>Time used: %li.%li sec</li>"
                  "</ul>",
                  service->statistics->requests,
                  service->statistics->bytes_received,
                  service->statistics->bytes_transmitted,
		  service->statistics->time.tv_sec,
		  service->statistics->time.tv_usec);
  pthread_rwlock_unlock(&(service->statistics->lock));

  http_output_stream_write_string(conn->out, buffer);

  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_enable_service(httpd_conn_t *conn, const char *service_name)
{
  hservice_t *service;
  char buffer[1024];

  sprintf(buffer, "Activating service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!(service = httpd_find_service(service_name)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");

    _httpd_admin_send_footer(conn);
    return;   
  }

  httpd_enable_service(service);

  http_output_stream_write_string(conn->out,
    "<p>"
      "Service is up"
    "</p>");

  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_disable_service(httpd_conn_t *conn, const char *service_name)
{
  hservice_t *service;
  char buffer[1024];

  sprintf(buffer, "Passivating service <b>%s</b>", service_name);
  _httpd_admin_send_title(conn, buffer);

  if (!(service = httpd_find_service(service_name)))
  {
    http_output_stream_write_string(conn->out,
      "<p>"
        "Service not found!"
      "</p>");
    _httpd_admin_send_footer(conn);
    return;
  }

  httpd_disable_service(service);

  http_output_stream_write_string(conn->out,
    "<p>"
      "Service is down"
    "</p>");
  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_set_loglevel(httpd_conn_t *conn, const char *loglevel)
{
  nanohttp_loglevel_t old;
  char buffer[256];
  char *tmp;

  if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_OFF_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_OFF);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_VERBOSE_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_VERBOSE);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_DEBUG_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_DEBUG);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_INFO_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_INFO);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_WARN_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_WARN);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_ERROR_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_ERROR);
  }
  else if (strcmp(loglevel, NANOHTTP_LOG_LEVEL_FATAL_STRING) == 0)
  {
    old = nanohttp_log_set_loglevel(NANOHTTP_LOG_FATAL);
  }
  else
  {
    old = nanohttp_log_get_loglevel();
    loglevel = NANOHTTP_LOG_LEVEL_UNKNOWN_STRING;
    log_error("unknown loglevel requested");
  }

  switch (old)
  {
    case NANOHTTP_LOG_OFF:
      tmp = NANOHTTP_LOG_LEVEL_OFF_STRING;
      break;
    case NANOHTTP_LOG_VERBOSE:
      tmp = NANOHTTP_LOG_LEVEL_VERBOSE_STRING;
      break;
    case NANOHTTP_LOG_DEBUG:
      tmp = NANOHTTP_LOG_LEVEL_DEBUG_STRING;
      break;
    case NANOHTTP_LOG_INFO:
      tmp = NANOHTTP_LOG_LEVEL_INFO_STRING;
      break;
    case NANOHTTP_LOG_WARN:
      tmp = NANOHTTP_LOG_LEVEL_WARN_STRING;
      break;
    case NANOHTTP_LOG_ERROR:
      tmp = NANOHTTP_LOG_LEVEL_ERROR_STRING;
      break;
    case NANOHTTP_LOG_FATAL:
      tmp = NANOHTTP_LOG_LEVEL_FATAL_STRING;
      break;
    default:
      tmp = NANOHTTP_LOG_LEVEL_UNKNOWN_STRING;
      break;
  }

  _httpd_admin_send_title(conn, "Adjusting loglevel");

  sprintf(buffer, "<p>Switching from %s to %s</p>", tmp, loglevel);
  http_output_stream_write_string(conn->out, buffer);

  _httpd_admin_send_footer(conn);
}

static void
_httpd_admin_handle_get(httpd_conn_t * conn, struct hrequest_t *req)
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
  else if ((param = hpairnode_get_ignore_case(req->query, NHTTPD_ADMIN_QUERY_ACTIVATE_SERVICE)))
  {
    _httpd_admin_enable_service(conn, param);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, NHTTPD_ADMIN_QUERY_PASSIVATE_SERVICE)))
  {
    _httpd_admin_disable_service(conn, param);
  }
  else if ((param = hpairnode_get_ignore_case(req->query, NHTTPD_ADMIN_QUERY_SET_LOGLEVEL)))
  {
    _httpd_admin_set_loglevel(conn, param);
  }
  else
  {
    _httpd_admin_send_title(conn, "Welcome to the admin page");

    http_output_stream_write_string(conn->out,
      "<ul>"
        "<li>"
          "<a href=\"?" NHTTPD_ADMIN_QUERY_SERVICES "\">Services</a>"
        "</li>"
        "<li>"
          "<a href=\"?" NHTTPD_ADMIN_QUERY_STATISTICS "\">Statistics</a>"
        "</li>"
	"<li>Set loglevel: "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_OFF_STRING "\">OFF</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_VERBOSE_STRING "\">VERBOSE</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_DEBUG_STRING "\">DEBUG</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_INFO_STRING "\">INFO</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_WARN_STRING "\">WARN</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_ERROR_STRING "\">ERROR</a> "
	  "<a href=\"?" NHTTPD_ADMIN_QUERY_SET_LOGLEVEL "=" NANOHTTP_LOG_LEVEL_FATAL_STRING "\">FATAL</a> "
	"</li>"
      "</ul>");

    _httpd_admin_send_footer(conn);
  }
}

static void
_httpd_admin_entry(httpd_conn_t * conn, struct hrequest_t *req)
{
  if (req->method == HTTP_REQUEST_GET)
  {
    _httpd_admin_handle_get(conn, req);
  }
  else
  {
    httpd_send_header(conn, 501, HTTP_STATUS_501_REASON_PHRASE);
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
httpd_admin_init_args(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++)
  {
    if (!strcmp(argv[i], NHTTPD_ARG_ENABLE_ADMIN))
    {
      httpd_register(NHTTPD_ADMIN_CONTEXT, _httpd_admin_entry);
      break;
    }
  }

  return H_OK;
}
