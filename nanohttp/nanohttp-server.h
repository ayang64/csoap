/******************************************************************
 *  $Id: nanohttp-server.h,v 1.2 2004/01/30 16:37:50 snowdrop Exp $
 *
 * CSOAP Project:  A http client/server library in C
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
#ifndef NANO_HTTP_SERVER_H 
#define NANO_HTTP_SERVER_H 


#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-socket.h>


#define NHTTPD_ARG_PORT "-NHTTPport"

typedef struct httpd_conn
{
  hsocket_t sock;
  char content_type[25];
}httpd_conn_t;


/*
  Service callback
 */
typedef void (*httpd_service)(httpd_conn_t*, hrequest_t*);


/*
 * Service representation object
 */
typedef struct tag_hservice
{
  char ctx[255];
  httpd_service func;
  struct tag_hservice *next;
}hservice_t;



/*
  Begin  httpd_* function set
 */
int httpd_init(int argc, char *argv[]);
int httpd_register(const char* ctx,  httpd_service service);
int httpd_run();
void httpd_destroy();

hservice_t *httpd_services();

int httpd_send_header(httpd_conn_t *res, 
		      int code, const char* text, 
		      hpair_t *pair);


char *httpd_get_postdata(httpd_conn_t *conn, 
			 hrequest_t *req, long *received, long max);

#endif

