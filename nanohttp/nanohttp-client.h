/******************************************************************
 *  $Id: nanohttp-client.h,v 1.3 2003/12/18 15:32:09 snowdrop Exp $
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
#ifndef NANO_HTTP_CLIENT_H 
#define NANO_HTTP_CLIENT_H 


#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-socket.h>


typedef struct httpc_conn
{
  hsocket_t sock;
  hpair_t *header;
  hurl_t *url;
}httpc_conn_t;

/*
  PROTOTYPE:
  void my_callback(int counter, httpc_conn_t* conn,
	    	   void *userdata, int size, char *buffer)
 */
typedef void (*httpc_response_callback)(int, httpc_conn_t*, void*,int,char*); 

/*
  void my_start_callback(httpc_conn_t *conn, void *userdata, 
		    hpair_t *header, const char *spec,
		    int errcode, const char *desc)
 */
typedef void (*httpc_response_start_callback)(httpc_conn_t*, void*, hpair_t*, 
					      const char*, int, const char*);

httpc_conn_t* httpc_new();
void httpc_free(httpc_conn_t* conn);

int httpc_set_header(httpc_conn_t *conn, const char* key, const char* value);

hresponse_t *httpc_get(httpc_conn_t *conn, const char *url);
hresponse_t *httpc_post(httpc_conn_t *conn, const char *url, 
			int conten_size, const char *content);

int httpc_get_cb(httpc_conn_t *conn, const char *url, 
		 httpc_response_start_callback start_cb,
		 httpc_response_callback cb, void *userdata);
		 

int httpc_post_cb(httpc_conn_t *conn, const char *url, 
		  httpc_response_start_callback start_cb,
		  httpc_response_callback cb, int content_size, 
		  char *content,  void *userdata);



int httpc_post_open(httpc_conn_t *conn);
int httpc_post_send(httpc_conn_t *conn, const char* buffer);
hresponse_t *httpc_post_finish(httpc_conn_t *conn);
int httpc_post_finish_cb(httpc_conn_t *conn, 
			 httpc_response_start_callback start_cb,
			 httpc_response_callback cb);


#endif
















