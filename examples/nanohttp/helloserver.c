/******************************************************************
 *  $Id: helloserver.c,v 1.1 2004/01/21 12:27:47 snowdrop Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#include <nanohttp/nanohttp-server.h>

#include <stdio.h>


/*
  SERVICE: http://host:port/hello
 */
void hello_world_service(httpd_conn_t *conn, hrequest_t *req)
{
  httpd_send_header(conn, 200, "OK", NULL);
  hsocket_send(conn->sock, "<html><body>");
  hsocket_send(conn->sock, "<h3>Hello World!</h3>");
  hsocket_send(conn->sock, "</body></html>");
}


int main(int argc, char *argv[])
{
  
  if (httpd_init(argc, argv)) {
    fprintf(stderr, "can not init httpd");
    return 1;
  }

  if (!httpd_register("/hello", hello_world_service)) {
    fprintf(stderr, "Can not register service");
    return 1;
  }

  if (httpd_run()) {
    fprintf(stderr, "can not run httpd");
    return 1;
  }

  return 0;
}








