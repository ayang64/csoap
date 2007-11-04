/******************************************************************
 *  $Id: nanohttp-server.h,v 1.40 2007/11/04 06:57:32 m0gg Exp $
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
#endif

/** @page nanohttp_server_page Howto write an HTTP server
 *
 * @section server_sec Table of contents
 * 
 * - @ref nanohttp_server_init_sec
 * - @ref nanohttp_server_service_sec
 * - @ref nanohttp_server_running_sec
 * - @ref nanohttp_server_cleanup_sec
 * - @ref nanohttp_server_function_sec
 *
 * @section nanohttp_server_init_sec Server initialization
 *
 * @code
 * int main(int argc, char **argv)
 * {
 *   herror_t status;
 *   nanohttp_set_level(NANOHTTP_LOG_INFO);
 *
 *   if (httpd_init(argc, argv))
 *   {
 *     fprintf(stderr, "Cannot init httpd\n");
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_service_sec Service registration
 *
 * @code
 *   if ((status = httpd_register("/", root_service)) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot register service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register_secure("/secure", secure_service, simple_authenticator)) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot register secure service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register("/headers", headers_service)) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot register headers service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((status = httpd_register_default("/error", default_service)) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot register default service (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_running_sec Running the server
 *
 * @code
 *   if ((status = httpd_run()) != H_OK)
 *   {
 *     fprintf(stderr, "Cannot run httpd (%s)\n", herror_message(status));
 *     herror_release(status);
 *     httpd_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section nanohttp_server_cleanup_sec Server cleanup
 *
 * @code
 *   httpd_destroy();
 *
 *   exit(0);
 * }
 * @endcode
 *
 * @section nanohttp_server_function_sec Sample service function
 *
 * @code
 * static void headers_service(httpd_conn_t *conn, struct hrequest_t *req)
 * {
 *   hpair_t *walker;
 *
 *   httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);
 *   http_output_stream_write_string(conn->out,
 *     "<html>"
 *       "<head>"
 *         "<title>Request headers</title>"
 *       "</head>"
 *       "<body>"
 *         "<h1>Request headers</h1>"
 *         "<ul>");
 *
 *   for (walker=req->header; walker; walker=walker->next)
 *   {
 *     http_output_stream_write_string(conn->out, "<li>");
 *     http_output_stream_write_string(conn->out, walker->key);
 *     http_output_stream_write_string(conn->out, " = ");
 *     http_output_stream_write_string(conn->out, walker->value);
 *     http_output_stream_write_string(conn->out, "</li>");
 *   }
 *
 *   http_output_stream_write_string(conn->out,
 *         "</ul>"
 *       "</body>"
 *     "</html>");
 *
 *   return;
 * }
 * @endcode
 *
 */

/** @file nanohttp-server.h HTTP server interface
 *
 * @defgroup NANOHTTP nanoHTTP
 */
/**@{*/

/** @defgroup NAONHTTP_CMDLINE_FLAGS Command line flags
 */
/**@{*/
/** @defgroup NANOHTTP_SERVER_CMDLINE_FLAGS Server command line flags
 */
/**@{*/
#define NHTTPD_ARG_PORT	   "-NHTTPport"     /**< Port the server is
					         listening on */
#define NHTTPD_ARG_TERMSIG "-NHTTPtsig"     /**< Signal which may used
					         to terminate the server */
#define NHTTPD_ARG_MAXCONN  "-NHTTPmaxconn" /**< Maximum number of
					         simultanous connections */
#define NHTTPD_ARG_TIMEOUT  "-NHTTPtimeout" /**< Timeout on reads */
/**@}*/
/**@}*/

/** @defgroup NANOHTTP_CLIENT Client */

/** @defgroup NANOHTTP_SERVER Server */
/**@{*/

typedef struct httpd_conn
{
  struct hsocket_t *sock;
  char content_type[25];
  struct http_output_stream_t *out;
  hpair_t *header;
}
httpd_conn_t;

/** Service callback function for a nanoHTTP service. */
typedef void (*httpd_service) (httpd_conn_t *conn, struct hrequest_t *req);

/** Authentication callback function for a nanoHTTP service. */
typedef int (*httpd_auth) (struct hrequest_t *req, const char *user, const char *pass);

#ifdef __NHTTP_INTERNAL
/** Service statistics per nanoHTTP service. */
struct service_statistics
{
  unsigned long requests;
  unsigned long bytes_transmitted;
  unsigned long bytes_received;
  struct timeval time;
  pthread_rwlock_t lock;
};
#endif

/** @defgroup NANOHTTP_SERVICE_STATUS Service status
 */ 
/**@{*/

/**
 * @see hservice_t
 * @see http://www.w3.org/TR/wslc/
 */
#define NHTTPD_SERVICE_DOWN	0

/**
 * @see hservice_t
 * @see http://www.w3.org/TR/wslc/
 */
#define NHTTPD_SERVICE_UP	1
/**@}*/

/** Service representation object
 */
typedef struct tag_hservice
{
  char *context;                         /**< Path where service is connected */
  int status;                            /**< Current status of this service */
  httpd_service func;                    /**< Service function */
  httpd_auth auth;                       /**< Authentication function */
  struct tag_hservice *next;             /**< Next service in service list */
  struct service_statistics *statistics; /**< Service statistics */
}
hservice_t;

#ifdef __cplusplus
extern "C"
{
#endif

/** This function initializes the nanoHTTP server.
 *
 * @see httpd_destroy()
 */
extern herror_t httpd_init(int argc, char *argv[]);

/** This function destroys the nanoHTTP server.
 *
 * @see httpd_init()
 */
extern void httpd_destroy(void);

/** This function executes the nanoHTTP server in an endless loop.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_run(void);

/** This function registers a service routine.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register(const char *context, httpd_service service);

/** This function registers a service routing which is secured by
 * a password (HTTP basic authentication).
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_secure(const char *context, httpd_service service, httpd_auth auth);

/** This function registers a service routing which is executed if
 * no matching service is found.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_default(const char *context, httpd_service service);

/** This function registers a serivce routing which is executed if
 * no matching service is found, it is protected by a password.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_register_default_secure(const char *context, httpd_service service, httpd_auth auth);

/** This function returns the port the service is listening on.
 *
 * @return The port the service is listening on.
 */
extern short httpd_get_port(void);

/** This function returns the timeout which is used for read's.
 *
 * @return The read timeout used.
 */
extern int httpd_get_timeout(void);

/** This function sets the timeout which is used for read.
 */
extern void httpd_set_timeout(int secs);

/** This function returns a string representation of the underlying
 * protocol used.
 *
 * @return "http" or "https"
 */
extern const char *httpd_get_protocol(void);

/** This function returns the actual connection count.
 */
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

/**
 *
 * @todo move to nanohttp-mime.c merge with httpc_mime_* functions
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

/** This function finishes a MIME request.
 *
 * @return H_OK on success.
 */
extern herror_t httpd_mime_end(httpd_conn_t * conn);

/** This function sends a minimalistic HTML error document with HTTP
 * status 400.
 *
 * @see HTTP_STATUS_400_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_bad_request(httpd_conn_t *conn, const char *msg);

/** This function sends a minimalistc HTML error document with HTTP
 * status 401.
 *
 * @see HTTP_STATUS_401_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_unauthorized(httpd_conn_t *conn, const char *realm);

/** This function sends a minimalistic HTML error document with HTTP
 * status 404.
 *
 * @see HTTP_STATUS_404_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_not_found(httpd_conn_t *conn, const char *msg);

/** This function sends a minimalistic HTML error document with HTTP
 * status 500.
 *
 * @see HTTP_STATUS_500_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_internal_error(httpd_conn_t * conn, const char *msg);

/** This functions sends a minimalistic HTML error document with HTTP
 * status 501.
 *
 * @see HTTP_STATUS_501_REASON_PHRASE
 *
 * @return H_OK on success.
 */
extern herror_t httpd_send_not_implemented(httpd_conn_t *conn, const char *msg);

#ifdef __cplusplus
}
#endif

/**@}*/
/**@}*/

#endif
