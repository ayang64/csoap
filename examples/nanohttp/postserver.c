/******************************************************************
 *  $Id: postserver.c,v 1.1 2004/01/30 16:38:49 snowdrop Exp $
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
  SERVICE: http://host:port/postserver
 */
void post_service(httpd_conn_t *conn, hrequest_t *req)
{
  char *postdata;
  long received;

  postdata = httpd_get_postdata(conn, req, &received, -1);

  if (postdata != NULL) {

    httpd_send_header(conn, 200, "OK", NULL);
    hsocket_send(conn->sock, "<html><body>");
    hsocket_send(conn->sock, "<h3>You Posted:</h3><hr>");
    hsocket_nsend(conn->sock, postdata, received);
    hsocket_send(conn->sock, "</body></html>");
    free(postdata);

  } else {

    httpd_send_header(conn, 200, "OK", NULL);
    hsocket_send(conn->sock, "<html><body>");
    hsocket_send(conn->sock, "<form action=\"/postserver\" method=\"POST\">");
    hsocket_send(conn->sock, "Enter Postdata: <input name=\"field\" type=\"text\">");
    hsocket_send(conn->sock, "<input type=\"submit\">");
    hsocket_send(conn->sock, "</body></html>");

  }


}


int main(int argc, char *argv[])
{
  
  if (httpd_init(argc, argv)) {
    fprintf(stderr, "can not init httpd");
    return 1;
  }

  if (!httpd_register("/postserver", post_service)) {
    fprintf(stderr, "Can not register service");
    return 1;
  }

  if (httpd_run()) {
    fprintf(stderr, "can not run httpd");
    return 1;
  }

  return 0;
}








