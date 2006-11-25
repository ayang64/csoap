/******************************************************************
*  $Id: http_server.c,v 1.9 2006/11/25 16:35:57 m0gg Exp $
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
#include <string.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-response.h>
#include <nanohttp/nanohttp-server.h>
#include <nanohttp/nanohttp-logging.h>

static int simple_authenticator(struct hrequest_t *req, const char *user, const char *password)
{

	log_info3("logging in user=\"%s\" password=\"%s\"", user, password);

	if (strcmp(user, "bob")) {

		log_warn2("user \"%s\" unkown", user);
		return 0;
	}

	if (strcmp(password, "builder")) {

		log_warn1("wrong password");
		return 0;
	}

	return 1;
}

static void secure_service(httpd_conn_t *conn, struct hrequest_t *req)
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

static void default_service(httpd_conn_t *conn, struct hrequest_t *req)
{

	httpd_send_header(conn, 404, HTTP_STATUS_404_REASON_PHRASE);
	http_output_stream_write_string(conn->out,
		"<html>"
			"<head>"
				"<title>Default error page</title>"
			"</head>"
			"<body>"
				"<h1>Default error page</h1>"
				"<div>");

	http_output_stream_write_string(conn->out, req->path);

	http_output_stream_write_string(conn->out, " can not be found"
		       		"</div>"
			"</body>"
		"</html>");

	return;
}	

static void headers_service(httpd_conn_t *conn, struct hrequest_t *req)
{
	hpair_t *walker;

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
		http_output_stream_write_string(conn->out, "<li>");
		http_output_stream_write_string(conn->out, walker->key);
		http_output_stream_write_string(conn->out, " = ");
		http_output_stream_write_string(conn->out, walker->value);
		http_output_stream_write_string(conn->out, "</li>");
	}

	http_output_stream_write_string(conn->out,
				"</ul>"
			"</body>"
		"</html>");

	return;
}

static void root_service(httpd_conn_t *conn, struct hrequest_t *req)
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
					"<li><a href=\"/not_existent\">The default service</a></li>"
					"<li><a href=\"/nhttp\">Admin page</a> (try -NHTTPDadmin on the command line)</li>"
				"</ul>"
			"</body>"
		"</html>");

	return;
}

int main(int argc, char **argv)
{
	hlog_set_level(HLOG_INFO);

	if (httpd_init(argc, argv)) {

		fprintf(stderr, "Cannot init httpd");
		return 1;
	}

	if (!httpd_register("/", root_service)) {

		fprintf(stderr, "Cannot register service");
		return 1;
	}

	if (!httpd_register_secure("/secure", secure_service, simple_authenticator)) {

		fprintf(stderr, "Cannot register secure service");
		return 1;
	}

	if (!httpd_register("/headers", headers_service)) {

		fprintf(stderr, "Cannot register headers service");
		return 1;
	}

	if (!httpd_register_default("/error", default_service)) {

		fprintf(stderr, "Cannot register default service");
		return 1;
	}

	if (httpd_run()) {

		fprintf(stderr, "can not run httpd");
		return 1;
	}

	httpd_destroy();

	return 0;
}
