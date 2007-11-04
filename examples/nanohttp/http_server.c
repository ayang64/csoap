/******************************************************************
*  $Id: http_server.c,v 1.16 2007/11/04 07:37:38 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C (example)
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
* Email: hero@persua.de
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nanohttp/nanohttp-logging.h>
#include <nanohttp/nanohttp-server.h>

static int
simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{
  fprintf(stdout, "logging in user=\"%s\" password=\"%s\"\n", user, password);

  if (strcmp(user, "bob")) {

    fprintf(stderr, "user \"%s\" unkown\n", user);
    return 0;
  }

  if (strcmp(password, "builder")) {

    fprintf(stderr, "wrong password\n");
    return 0;
  }

  return 1;
}

static void
secure_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Secure ressource!</title>"
      "</head>"
      "<body>"
        "<h1>Authenticated access!!!</h1>"
      "</body>"
    "</html>");

  return;
}

static void
default_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  char buf[512];

  snprintf(buf, 512, "Resource \"%s\" not found", req->path);

  httpd_send_not_found(conn, buf);

  return;
}

static void
headers_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  hpair_t *walker;
  char buf[512];
  int len;

  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>Request headers</title>"
      "</head>"
      "<body>"
        "<h1>Request headers</h1>"
        "<ul>");

  for (walker=req->header; walker; walker=walker->next)
  {
    len = snprintf(buf, 512, "<li>%s: %s</li>", walker->key, walker->value);
    http_output_stream_write(conn->out, buf, len);
  }

  http_output_stream_write_string(conn->out,
        "</ul>"
      "</body>"
    "</html>");
}

static void
mime_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_not_implemented(conn, "mime_service");
}

static void
post_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  if (req->method == HTTP_REQUEST_POST)
  {
    unsigned char buffer[1024];
    long len, total;

    httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
    http_output_stream_write_string(conn->out,
      "<html>"
        "<head>"
	  "<title>POST service</title>"
        "</head>"
        "<body>"
          "<h1>You posted</h1>"
          "<pre>");

    if (req->content_type && req->content_type->type)
    {
      len = sprintf(buffer, "<p>Content-Type: %s</p>", req->content_type->type);
      http_output_stream_write(conn->out, buffer, len);
    }

    while (http_input_stream_is_ready(req->in))
    {
	    len = http_input_stream_read(req->in, buffer, 1024);
	    http_output_stream_write(conn->out, buffer, len);
	    total += len;
    }

    http_output_stream_write_string(conn->out,
          "</pre>");

    len = sprintf(buffer, "<p>Received %li bytes</p>", total);
    http_output_stream_write(conn->out, buffer, len);

    http_output_stream_write_string(conn->out,
        "</body>"
      "</html>");
  }
  else
  {
    httpd_send_not_implemented(conn, "post_service");
  }
}

static void
root_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
  http_output_stream_write_string(conn->out,
    "<html>"
      "<head>"
        "<title>nanoHTTP server examples</title>"
      "</head>"
      "<body>"
        "<h1>nanoHTTP server examples</h1>"
        "<ul>"
          "<li><a href=\"/\">Simple service</a></li>"
          "<li><a href=\"/secure\">Secure service</a> (try: bob/builder)</li>"
          "<li><a href=\"/headers\">Request headers</a></li>"
	  "<li><a href=\"/mime\">MIME service</a></li>"
	  "<li>"
            "<form action=\"/post\" method=\"post\" enctype=\"multipart/form-data\">"
              "<fieldset>"
                "<legend>Upload file</legend>"
                "<input name=\"Text\" type=\"text\" value=\"test-field\"? "
                "<input name=\"File\" type=\"file\" accept=\"text/*\"> "
                "<input value=\"Submit\" type=\"submit\">"
              "</fieldset>"
            "</form>"
          "</li>"
          "<li><a href=\"/not_existent\">The default service</a></li>"
          "<li><a href=\"/nhttp\">Admin page</a> (try -NHTTPDadmin on the command line)</li>"
        "</ul>"
      "</body>"
    "</html>");
}

int
main(int argc, char **argv)
{
  herror_t status;

  nanohttp_log_set_loglevel(NANOHTTP_LOG_INFO);

  if (httpd_init(argc, argv))
  {
    fprintf(stderr, "Cannot init httpd\n");
    exit(1);
  }

  if ((status = httpd_register("/", root_service)) != H_OK)
  {
    fprintf(stderr, "Cannot register service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  if ((status = httpd_register_secure("/secure", secure_service, simple_authenticator)) != H_OK)
  {
    fprintf(stderr, "Cannot register secure service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  if ((status = httpd_register("/headers", headers_service)) != H_OK)
  {
    fprintf(stderr, "Cannot register headers service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  if ((status = httpd_register("/mime", mime_service)) != H_OK)
  {
    fprintf(stderr, "Cannot register MIME service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1); 
  }

  if ((status = httpd_register("/post", post_service)) != H_OK)
  {
    fprintf(stderr, "Cannot register POST service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  if ((status = httpd_register_default("/error", default_service)) != H_OK)
  {
    fprintf(stderr, "Cannot register default service (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  if ((status = httpd_run()) != H_OK)
  {
    fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
    herror_release(status);
    httpd_destroy();
    exit(1);
  }

  httpd_destroy();

  return 0;
}
