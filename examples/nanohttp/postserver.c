/******************************************************************
*  $Id: postserver.c,v 1.3 2004/09/19 07:05:03 snowdrop Exp $
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

static 
void _print_binary_ascii(int n)
{
  int i,c=0;
  char ascii[36];

  for (i=0;i<32;i++) {
    ascii[34-i-c] = (n & (1<<i))?'1':'0';
    if ((i+1)%8 == 0) {
        c++;
        ascii[i+c] = ' ';
    }
  }

  ascii[35]='\0';

  log_verbose2("%s", ascii);
}


static 
void _print_binary_ascii2(unsigned char n)
{
  int i,c=0;
  char ascii[9];

  for (i=0;i<8;i++) {
    ascii[7-i] = (n & (1<<i))?'1':'0';
  }

  ascii[8]='\0';

  log_verbose2("%s", ascii);
}

/*
SERVICE: http://host:port/postserver
*/
void post_service(httpd_conn_t *conn, hrequest_t *req)
{
	unsigned char *postdata;
	long received;
	unsigned int tmp;
	char buffer[15];

	postdata = httpd_get_postdata(conn, req, &received, -1);

	if (postdata != NULL) {

		httpd_send_header(conn, 200, "OK", NULL);
		hsocket_send(conn->sock, "<html><body>\n");
		hsocket_send(conn->sock, "<h3>You Posted:</h3><hr>\n");
		hsocket_nsend(conn->sock, postdata, received);
		hsocket_send(conn->sock, "<h3>Received size</h3><hr>\n");
		sprintf(buffer, "%d", received);
		hsocket_send(conn->sock, buffer);
		hsocket_send(conn->sock, "</body></html>");

		_print_binary_ascii2(postdata[0]);
		_print_binary_ascii2(postdata[1]);
		_print_binary_ascii2(postdata[2]);
		_print_binary_ascii2(postdata[3]);
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
	log_set_level(HLOG_VERBOSE);

	if (httpd_init(argc, argv)) {
		fprintf(stderr, "can not init httpd");
		return 1;
	}

	if (!httpd_register("/", post_service)) {
		fprintf(stderr, "Can not register service");
		return 1;
	}

	if (httpd_run()) {
		fprintf(stderr, "can not run httpd");
		return 1;
	}

	return 0;
}
