/******************************************************************
 *  $Id: nanohttp-client.h,v 1.23 2006/03/06 13:37:38 m0gg Exp $
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
     -1 : last dime package 0 : no dime connection >0 : dime package number */
  int _dime_package_nr;
  long _dime_sent_bytes;
  int errcode;
  char errmsg[150];
  http_output_stream_t *out;
  int id;                       /* uniq id */
} httpc_conn_t;


#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------
 HTTP CLIENT MODULE RELATED FUNCTIONS
 ---------------------------------------------------------------*/

/**
  initialize the httpc_* module
*/
herror_t httpc_init(int argc, char *argv[]);

/**
  Destroy the httpc_* module
*/
void httpc_destroy();

/**
  Creates a new connection
*/
httpc_conn_t *httpc_new();

/**
  Close and release a connection
*/
void httpc_close_free(httpc_conn_t * conn);

/**
  Release a connection
  (use httpc_close_free() instead)
*/
void httpc_free(httpc_conn_t * conn);

/**
 * Close and release a connection
 */
void httpc_close_free(httpc_conn_t * conn);

/**
  Set header element (key,value) pair.
*/
int httpc_set_header(httpc_conn_t * conn, const char *key, const char *value);

int httpc_add_header(httpc_conn_t *conn, const char *key, const char *value);
void httpc_add_headers(httpc_conn_t *conn, const hpair_t *values);

int httpc_set_basic_authorization(httpc_conn_t *conn, const char *user, const char *password);
int httpc_set_basic_proxy_authorization(httpc_conn_t *conn, const char *user, const char *password);

/**
  Invoke a "GET" method request and receive the response
*/
herror_t httpc_get(httpc_conn_t * conn, hresponse_t ** out, const char *urlstr);

/**
  Start a "POST" method request
  Returns: HSOCKET_OK  or error flag
*/
herror_t httpc_post_begin(httpc_conn_t * conn, const char *url);

/**
  End a "POST" method and receive the response.
  You MUST call httpc_post_end() before!
*/
herror_t httpc_post_end(httpc_conn_t * conn, hresponse_t ** out);


/* --------------------------------------------------------------
 DIME RELATED FUNCTIONS
 ---------------------------------------------------------------*/

/*
  DIME support httpc_dime_* function set
*/
/*
int httpc_dime_begin(httpc_conn_t *conn, const char *url);
int httpc_dime_next(httpc_conn_t* conn, long content_length, 
                    const char *content_type, const char *id,
                    const char *dime_options, int last);
hresponse_t* httpc_dime_end(httpc_conn_t *conn);
*/

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
herror_t httpc_mime_begin(httpc_conn_t * conn, const char *url,
                          const char *related_start,
                          const char *related_start_info,
                          const char *related_type);

/**
  Send boundary and part header and continue 
  with next part
*/
herror_t httpc_mime_next(httpc_conn_t * conn,
                         const char *content_id,
                         const char *content_type,
                         const char *transfer_encoding);

/**
  Finish MIME request and get the response
*/
herror_t httpc_mime_end(httpc_conn_t * conn, hresponse_t ** out);

/**
  Send boundary and part header and continue 
  with next part
*/

herror_t httpc_mime_send_file(httpc_conn_t * conn,
                              const char *content_id,
                              const char *content_type,
                              const char *transfer_encoding,
                              const char *filename);

#ifdef __cplusplus
}
#endif

#endif
