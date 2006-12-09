/******************************************************************
 *  $Id: nanohttp-server.h,v 1.28 2006/12/09 09:57:38 m0gg Exp $
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
#ifndef __nanohttp_server_h
#define __nanohttp_server_h

/* XXX: Clean up nanohttp to make this unnecessary */
#ifndef __NHTTP_INTERNAL
#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-response.h>
#include <nanohttp/nanohttp-logging.h>
#endif

/**
 *
 * @page nanoHTTP nanoHTTP
 *
 * \section nanohttp_sec nanoHTTP
 *
 * nanoHTTP is an embedded HTTP implementation. It comes with the following
 * features:
 * - client/server HTTP engnine
 * - attachments via MIME
 * - HTTPS (SSL/TLS) via OpenSSL
 *
 * @version 1.2
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 * @see http://www.openssl.org
 *
 */

/**
 *
 * nanohttp command line flags
 *
 */
#define NHTTPD_ARG_PORT		"-NHTTPport"
#define NHTTPD_ARG_TERMSIG	"-NHTTPtsig"
#define NHTTPD_ARG_MAXCONN	"-NHTTPmaxconn"
#define NHTTPD_ARG_TIMEOUT	"-NHTTPtimeout"

typedef struct httpd_conn
{
  struct hsocket_t *sock;
  char content_type[25];
  struct http_output_stream_t *out;
  hpair_t *header;
}
httpd_conn_t;

/**
 *
 * Service callback function for a nanoHTTP service.
 *
 */
typedef void (*httpd_service) (httpd_conn_t *conn, struct hrequest_t *req);

/**
 *
 * Authentication callback function for a nanoHTTP service.
 *
 */
typedef int (*httpd_auth) (struct hrequest_t *req, const char *user, const char *pass);

#ifdef __NHTTP_INTERNAL
/**
 *
 * Service statistics per nanoHTTP service.
 *
 */
struct service_statistics
{
  unsigned long requests;
  unsigned long bytes_transmitted;
  unsigned long bytes_received;
  struct timeval time;
  pthread_rwlock_t lock;
};
#endif

/**
 *
 * @see hservice_t
 *
 */
#define NHTTPD_SERVICE_DISABLED	0

/**
 *
 * @see hservice_t
 *
 */
#define NHTTPD_SERVICE_ENABLED	1

/**
 *
 * Service representation object
 *
 */
typedef struct tag_hservice
{
  char ctx[255];
  int status;
  httpd_service func;
  httpd_auth auth;
  struct tag_hservice *next;
  struct service_statistics *statistics;
}
hservice_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 * Initialize the nanoHTTP server.
 *
 */
extern herror_t httpd_init(int argc, char *argv[]);

/**
 *
 * @see httpd_init
 *
 */
extern void httpd_destroy(void);


extern herror_t httpd_run(void);

extern herror_t httpd_register(const char *ctx, httpd_service service);
extern herror_t httpd_register_secure(const char *ctx, httpd_service service, httpd_auth auth);

extern herror_t httpd_register_default(const char *ctx, httpd_service service);
extern herror_t httpd_register_default_secure(const char *ctx, httpd_service service, httpd_auth auth);

extern short httpd_get_port(void);
extern int httpd_get_timeout(void);
extern void httpd_set_timeout(int secs);

extern const char *httpd_get_protocol(void);
extern int httpd_get_conncount(void);

extern hservice_t *httpd_get_services(void);
extern hservice_t *httpd_find_service(const char *name);

extern int httpd_enable_service(hservice_t *service);
extern int httpd_disable_service(hservice_t *service);

extern void httpd_response_set_content_type(httpd_conn_t * res, const char *content_type);

extern herror_t httpd_send_header(httpd_conn_t * res, int code, const char *text);

extern int httpd_set_header(httpd_conn_t * conn, const char *key, const char *value);
extern void httpd_set_headers(httpd_conn_t * conn, hpair_t * header);

extern int httpd_add_header(httpd_conn_t * conn, const char *key, const char *value);
extern void httpd_add_headers(httpd_conn_t * conn, const hpair_t * values);

/*
 * XXX: move to nanohttp-mime.c
 *
 * MIME support httpd_mime_* function set
 */

/**
 *
 * MIME multipart/related POST 
 * @returns H_OK on success or error flag
 *
 */
extern herror_t httpd_mime_send_header(httpd_conn_t * conn, const char *related_start, const char *related_start_info, const char *related_type, int code, const char *text);

/**
 *
 * Send boundary and part header and continue with next part
 *
 */
extern herror_t httpd_mime_next(httpd_conn_t * conn, const char *content_id, const char *content_type, const char *transfer_encoding);

/**
 *
 * Send boundary and part header and continue with next part
 *
 */
extern herror_t httpd_mime_send_file(httpd_conn_t * conn, const char *content_id, const char *content_type, const char *transfer_encoding, const char *filename);

/**
 *
 * Finish MIME request 
 *
 * @return H_OK on success or error flag
 *
 */
extern herror_t httpd_mime_end(httpd_conn_t * conn);

#ifdef __cplusplus
}
#endif

#endif
