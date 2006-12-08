/******************************************************************
 *  $Id: nanohttp-client.h,v 1.30 2006/12/08 21:21:41 m0gg Exp $
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
#ifndef __nanohttp_client_h
#define __nanohttp_client_h

/* XXX: Clean up nanohttp to make this unnecessary */
#ifndef __NHTTP_INTERNAL
#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-response.h>
#include <nanohttp/nanohttp-logging.h>
#endif

/** @file
 *
 * Writing an HTTP client using nanoHTTP
 *
 * - Client initialization
 * - Connection initialization
 * -- SSL related functions
 * - Setting HTTP headers (optional)
 * -- Setting an HTTP header with uniq key
 * -- Setting multiple headers
 * - Fetch the network resource
 * -- HTTP GET command
 * -- HTTP POST command
 * -- MIME attachments
 * - Print out the result
 * - Connection cleanup
 * - Client cleanup
 *
 * Client initialization
 *
 * int main(int argc, char **argv)
 * {
 *   herror_t status;
 *   size_t len;
 *   httpc_conn_t conn;
 *
 *   if (argc < 2)
 *   {
 *     fprintf(stderr, "usage: %s <url> [nanoHTTP params]\n", argv[0]);
 *     exit(1);
 *   }
 *
 *   if ((status = httpc_init(argc, argv)) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot init nanoHTTP client (%s)\n", herror_message(status));
 *     herror_release(status);
 *     exit(1);
 *   }
 *
 * Connection initialization
 *
 *   if (!(conn = httpc_new()))
 *   {
 *     fprintf(stderr, "Cannot create nanoHTTP client connection\n");
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 * SSL related functions
 *
 * T.B.D.
 *
 * Setting HTTP headers (optional)
 *
 *   httpc_set_header(conn, "my-key", "my-value");
 *
 *   httpc_add_header(conn, "Cookie", "name1:value1");
 *   httpc_add_header(conn, "Cookie", "name2:value2");
 *
 * Fetch the network resource
 *
 * HTTP GET command
 *
 *   if ((status = httpc_get(conn, &result, argv[1])) != H_OK)
 *   {
 *     fprintf(stderr, "nanoHTTP client connection failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 * HTTP POST command
 *
 *   if ((status = httpc_post_begin(conn, argv[1])) != H_OK)
 *   {
 *     fprintf(stderr, "nanoHTTP client connection failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 *   if ((status = http_output_stream_write(conn->out, buffer, len)) != H_OK)
 *   {
 *     fprintf(stderr, "nanoHTTP client sending POST data failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 *   if ((status = httpc_post_end(conn, &result)) != H_OK)
 *   {
 *     fprintf(stderr, "nanoHTTP client POST failed (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpc_destroy();
 *     exit(1);
 *   }
 *
 * MIME attachments
 *
 * T.B.D.
 *
 * Print out the result
 *
 *   while (http_input_stream_is_read(res->in))
 *   {
 *     len = http_input_stream_read(res->in, buffer, MAX_BUFFER_SIZE);
 *     fwrite(buffer, len, 1, stdout);
 *   }
 *
 * Connection cleanup
 *
 *   hresponse_free(res);
 *
 * Client cleanup
 *
 *   httpc_free(conn);
 *
 * }
 *
 */

typedef struct httpc_conn
{
  struct hsocket_t *sock;
  hpair_t *header;
  struct hurl_t *url;
  http_version_t version;

  int errcode;
  char errmsg[150];
  struct http_output_stream_t *out;
  int id;                       /* uniq id */
} httpc_conn_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the httpc_* module. This is called from
 * soap_client_init_args().
 *
 * @param argc		Argument count.
 * @param argv		Argument vector.
 *
 * @return H_OK on succes or a herror_t struct on failure.
 *
 * @see httpc_destroy, herror_t, soap_client_init_args
 */
extern herror_t httpc_init(int argc, char **argv);

/**
 *
 * Destroys the httpc_* module
 *
 * @see httpc_init
 *
 */
extern void httpc_destroy(void);

/**
 *
 * Creates a new http client connection object. You need to create at least one
 * http client connection to communicate via HTTP.
 *
 * @return A pointer to a httpc_conn_t structure on success, NULL on error.
 *
 * @see httpc_conn_t
 *
 */
extern httpc_conn_t *httpc_new(void);

/**
 *
 * Release a connection
 * (use httpc_close_free() instead)
 *
 * @see httpc_close_free
 *
 */
extern void httpc_free(httpc_conn_t * conn);

/**
 *
 * Close and release a connection
 *
 * @see httpc_close
 * @see httpc_free
 *
 */
extern void httpc_close_free(httpc_conn_t * conn);

/**
 *
 * Sets header element (key,value) pair.
 *
 * @return 0 on success or failure (yeah!), 1 if a (key,value) pair was replaced.
 *
 * @see httpc_add_header
 * @see httpc_add_headers
 *
 */
extern int httpc_set_header(httpc_conn_t * conn, const char *key, const char *value);

/**
 *
 * Adds a header element (key, value) pair.
 *
 * @return 0 on success, -1 on failure.
 *
 * @see httpc_set_header
 * @see httpc_add_headers
 *
 */
extern int httpc_add_header(httpc_conn_t *conn, const char *key, const char *value);

/**
 *
 * Adds a list of (key, value) pairs.
 *
 * @see httpc_set_header
 * @see httpc_add_header
 *
 */
extern void httpc_add_headers(httpc_conn_t *conn, const hpair_t *values);

/**
 *
 * Sets a HEADER_AUTHORIZATION header.
 *
 * @param conn		The HTTP connection.
 * @param user		The username.
 * @param password	The password.
 *
 * @see httpc_set_header
 * @see HEADER_AUTHORIZATION
 *
 */
extern int httpc_set_basic_authorization(httpc_conn_t *conn, const char *user, const char *password);

/**
 *
 * Sets a HEADER_PROXY_AUTHORIZATION header.
 *
 * @param conn		The HTTP connection.
 * @param user		The username.
 * @param password	The password.
 *
 * @see httpc_set_header
 * @see HEADER_PROXY_AUTHORIZATION
 *
 */
extern int httpc_set_basic_proxy_authorization(httpc_conn_t *conn, const char *user, const char *password);

/**
 *
 * Invoke a "GET" method request and receive the response
 *
 * @return H_OK on success
 *
 */
extern herror_t httpc_get(httpc_conn_t * conn, hresponse_t ** out, const char *urlstr);

/**
 *
 * Start a "POST" method request
 *
 * @return H_OK on success or error flag
 *
 */
extern herror_t httpc_post_begin(httpc_conn_t * conn, const char *url);

/**
 *
 * End a "POST" method and receive the response.
 * You MUST call httpc_post_end() before!
 *
 * @return H_OK on success
 *
 * @see httpc_post_end
 *
 */
extern herror_t httpc_post_end(httpc_conn_t * conn, hresponse_t ** out);

/**
 *
 * Begin MIME multipart/related POST request
 *
 * @return H_OK on success or error flag
 *
 */
extern herror_t httpc_mime_begin(httpc_conn_t * conn, const char *url,
                          const char *related_start,
                          const char *related_start_info,
                          const char *related_type);

/**
 *
 * Send boundary and part header and continue with next part
 *
 * @return H_OK on success
 *
 */
extern herror_t httpc_mime_next(httpc_conn_t * conn,
                         const char *content_id,
                         const char *content_type,
                         const char *transfer_encoding);

/**
 *
 * Finish MIME request and get the response
 *
 * @return H_OK on success
 *
 */
extern herror_t httpc_mime_end(httpc_conn_t * conn, hresponse_t ** out);

/**
 *
 * Send boundary and part header and continue with next part
 *
 * @return H_OK on success
 *
 */
extern herror_t httpc_mime_send_file(httpc_conn_t * conn,
                              const char *content_id,
                              const char *content_type,
                              const char *transfer_encoding,
                              const char *filename);

#ifdef __cplusplus
}
#endif

#endif
