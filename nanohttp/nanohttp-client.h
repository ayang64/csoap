/******************************************************************
 *  $Id: nanohttp-client.h,v 1.10 2004/10/15 15:10:37 snowdrop Exp $
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef NANO_HTTP_CLIENT_H 
#define NANO_HTTP_CLIENT_H 


#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-socket.h>
#include <nanohttp/nanohttp-response.h>
#include <nanohttp/nanohttp-stream.h>

typedef struct httpc_conn
{
  hsocket_t sock;
  hpair_t *header;
  hurl_t url;
  http_version_t version;
  /*
    -1  : last dime package 
    0   : no dime connection
    >0  : dime package number
  */
  int _dime_package_nr;
  long _dime_sent_bytes; 
  int errcode;
  char errmsg[150];
  http_output_stream_t *out;
  int id; /* uniq id */
}httpc_conn_t;


/* --------------------------------------------------------------
 HTTP CLIENT MODULE RELATED FUNCTIONS
 ---------------------------------------------------------------*/

/**
  initialize the httpc_* module
*/
hstatus_t httpc_init(int argc, char *argv[]);
 
/**
  Creates a new connection
*/
httpc_conn_t* httpc_new();

/**
  Release a connections
*/
void httpc_free(httpc_conn_t* conn);

/**
  Set header element (key,value) pair.
*/
int httpc_set_header(httpc_conn_t *conn, const char* key, const char* value);

/**
  Invoke a "GET" method request and receive the response
*/
hresponse_t *httpc_get(httpc_conn_t *conn, const char *url);

/**
  Start a "POST" method request
  Returns: HSOCKET_OK  or error flag
*/
int httpc_post_begin(httpc_conn_t *conn, const char *url);

/**
  End a "POST" method and receive the response.
  You MUST call httpc_post_end() before!
*/
hresponse_t *httpc_post_end(httpc_conn_t *conn);


/* --------------------------------------------------------------
 DIME RELATED FUNCTIONS
 ---------------------------------------------------------------*/

/*
  DIME support httpc_dime_* function set
*/
int httpc_dime_begin(httpc_conn_t *conn, const char *url);
int httpc_dime_next(httpc_conn_t* conn, long content_length, 
                    const char *content_type, const char *id,
                    const char *dime_options, int last);
hresponse_t* httpc_dime_end(httpc_conn_t *conn);


/* --------------------------------------------------------------
 MIME RELATED FUNCTIONS
 ---------------------------------------------------------------*/
/*
  MIME support httpc_mime_* function set
*/

/**
  Begin MIME multipart/related POST request
  Returns: HSOCKET_OK  or error flag
*/
int httpc_mime_begin(httpc_conn_t *conn, const char *url,
  const char* related_start, 
  const char* related_start_info, 
  const char* related_type);

/**
  Send boundary and part header and continue 
  with next part
*/
int httpc_mime_next(httpc_conn_t *conn, 
  const char* content_id,
  const char* content_type, 
  const char* transfer_encoding);

/**
  Finish MIME request and get the response
*/
hresponse_t *httpc_mime_end(httpc_conn_t *conn);

/**
  Send boundary and part header and continue 
  with next part
*/
int
httpc_mime_send_file (httpc_conn_t * conn,
                      const char *content_id,
                      const char *content_type,
                      const char *transfer_encoding, 
                      const char *filename);

#endif


