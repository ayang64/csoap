/******************************************************************
*  $Id: http_server.c,v 1.1 2006/02/19 08:45:13 snowdrop Exp $
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

#include <nanohttp/nanohttp-server.h>

static int simple_authenticator(const char *user, const char *password)
{
	log_verbose3("loggin in user=\"%s\" password=\"%s\"", user, password);

	return 1;
}

static void simple_service(httpd_conn_t *conn, hrequest_t *req)
{

	httpd_send_header(conn, 200, "OK");
	hsocket_send(conn->sock, "<html><head><title>Success!</title></head><body><h1>Success!!!</h1></body></html>");

	return;
}

int main(int argc, char *argv[])
{
	log_set_level(HLOG_VERBOSE);

	if (httpd_init(argc, argv)) {

		fprintf(stderr, "can not init httpd");
		return 1;
	}

	if (!httpd_register_secure("/", simple_service, simple_authenticator))
	{

		fprintf(stderr, "Can not register service");
		return 1;
	}

	if (httpd_run()) {

		fprintf(stderr, "can not run httpd");
		return 1;
	}

	httpd_destroy();

	return 0;
}
