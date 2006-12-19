/******************************************************************
*  $Id: nanohttp-server.c,v 1.77 2006/12/19 08:55:17 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_PROCESS_H
#include <process.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-stream.h"
#include "nanohttp-request.h"
#include "nanohttp-response.h"
#include "nanohttp-server.h"
#include "nanohttp-base64.h"
#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#endif
#include "nanohttp-ssl.h"
#else
static inline int hssl_enabled(void) { return 0; }
#endif
#include "nanohttp-admin.h"

#ifndef timeradd
#define timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;	\
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#endif

#ifndef timersub
#define timersub(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif

typedef struct _conndata
{
  volatile int flag;
  struct hsocket_t sock;
#ifdef WIN32
  HANDLE tid;
#else
  pthread_t tid;
  pthread_attr_t attr;
#endif
}
conndata_t;

#define CONNECTION_FREE		0
#define CONNECTION_IN_USE	1

/**
 *
 * nanohttpd internally globals
 *
 */
static volatile int _httpd_run = 1;

static struct hsocket_t _httpd_socket;
static int _httpd_port = 10000;
static int _httpd_max_connections = 20;

static hservice_t *_httpd_services_default = NULL;
static hservice_t *_httpd_services_head = NULL;
static hservice_t *_httpd_services_tail = NULL;

static conndata_t *_httpd_connection;

#ifdef WIN32
static DWORD _httpd_terminate_signal = CTRL_C_EVENT;
static int _httpd_max_idle = 120;
HANDLE _httpd_connection_lock;
LPCTSTR _httpd_connection_lock_str;
#define strncasecmp(s1, s2, num) strncmp(s1, s2, num)
#define snprintf(buffer, num, s1, s2) sprintf(buffer, s1,s2)
#else
static int _httpd_terminate_signal = SIGINT;
static sigset_t thrsigset;
static pthread_mutex_t _httpd_connection_lock;
#endif

#ifdef WIN32
BOOL WINAPI
_httpd_term(DWORD sig)
{
  /* log_debug2 ("Got signal %d", sig); */
  if (sig == _httpd_terminate_signal)
    _httpd_run = 0;

  return TRUE;
}

static void _httpd_sys_sleep(int secs)
{
  Sleep(secs*1000);

  return;
}
#else
static void
_httpd_term(int sig)
{
  log_debug2("Got signal %d", sig);

  if (sig == _httpd_terminate_signal)
    _httpd_run = 0;

  return;
}

static inline void _httpd_sys_sleep(int secs)
{
  sleep(secs);

  return;
}
#endif

static void
_httpd_parse_arguments(int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i - 1], NHTTPD_ARG_PORT))
    {
      _httpd_port = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_TERMSIG))
    {
      _httpd_terminate_signal = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_MAXCONN))
    {
      _httpd_max_connections = atoi(argv[i]);
    }
    else if (!strcmp(argv[i - 1], NHTTPD_ARG_TIMEOUT))
    {
      hsocket_set_timeout(atoi(argv[i]));
    }
  }

  log_verbose2("socket bind to port '%d'", _httpd_port);

  return;
}


static void
_httpd_connection_slots_init(void)
{
  int i;

#ifdef WIN32
  _httpd_connection_lock = CreateMutex( NULL, TRUE, _httpd_connection_lock_str );
#else
  pthread_mutex_init(&_httpd_connection_lock, NULL);
#endif

  _httpd_connection = calloc(_httpd_max_connections, sizeof(conndata_t));
  for (i = 0; i < _httpd_max_connections; i++)
    hsocket_init(&(_httpd_connection[i].sock));

  return;
}

static herror_t
_httpd_register_builtin_services(int argc, char **argv)
{
  herror_t status;

  if ((status = httpd_admin_init_args(argc, argv)) != H_OK)
  {
    log_error2("httpd_admin_init_args failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
httpd_init(int argc, char **argv)
{
  herror_t status;

  _httpd_parse_arguments(argc, argv);

  if ((status = hsocket_module_init(argc, argv)) != H_OK)
  {
    log_error2("hsocket_modeule_init failed (%s)", herror_message(status));
    return status;
  } 

  _httpd_connection_slots_init();

  if ((status = _httpd_register_builtin_services(argc, argv)) != H_OK)
  {
    log_error2("_httpd_register_builtin_services failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_init(&_httpd_socket)) != H_OK)
  {
    log_error2("hsocket_init failed (%s)", herror_message(status));
    return status;
  }

  if ((status = hsocket_bind(&_httpd_socket, _httpd_port)) != H_OK)
  {
    log_error2("hsocket_bind failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
httpd_register_secure(const char *ctx, httpd_service func, httpd_auth auth)
{
  hservice_t *service;

  if (!(service = (hservice_t *) malloc(sizeof(hservice_t))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return herror_new("httpd_register_secure", 0, "malloc failed (%s)", strerror(errno));
  }

  if (!(service->statistics = (struct service_statistics *)malloc(sizeof(struct service_statistics))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    free(service);
    return herror_new("httpd_register_secure", 0, "malloc failed (%s)", strerror(errno));
  }    	
  memset(service->statistics, 0, sizeof(struct service_statistics));
  service->statistics->time.tv_sec = 0;
  service->statistics->time.tv_usec = 0;
  pthread_rwlock_init(&(service->statistics->lock), NULL);

  service->next = NULL;
  service->auth = auth;
  service->func = func;
  service->status = NHTTPD_SERVICE_UP;
  strcpy(service->ctx, ctx);

  log_verbose3("register service (%p) for \"%s\"", service, SAVE_STR(ctx));
  if (_httpd_services_head == NULL)
  {
    _httpd_services_head = _httpd_services_tail = service;
  }
  else
  {
    _httpd_services_tail->next = service;
    _httpd_services_tail = service;
  }

  return H_OK;
}

herror_t
httpd_register(const char *ctx, httpd_service service)
{
  return httpd_register_secure(ctx, service, NULL);
}

herror_t
httpd_register_default_secure(const char *ctx, httpd_service service, httpd_auth auth)
{
  herror_t ret;

  ret = httpd_register_secure(ctx, service, auth);

  /* XXX: this is broken, but working */
  _httpd_services_default = _httpd_services_tail;

  return ret;
}

herror_t
httpd_register_default(const char *ctx, httpd_service service)
{
  return httpd_register_default_secure(ctx, service, NULL);
}

short
httpd_get_port(void)
{
  return _httpd_port;
}

int
httpd_get_timeout(void)
{
  return hsocket_get_timeout();
}

void
httpd_set_timeout(int secs)
{
  hsocket_set_timeout(secs);

  return;
}

const char *
httpd_get_protocol(void)
{
  return hssl_enabled() ? "https" : "http";
}

int
httpd_get_conncount(void)
{
  int i, ret;

  for (ret = i = 0; i<_httpd_max_connections; i++)
  {
    if (_httpd_connection[i].flag == CONNECTION_IN_USE)
      ret++;
  }

  return ret;
}

hservice_t *
httpd_get_services(void)
{
  return _httpd_services_head;
}

static void
hservice_free(hservice_t * service)
{
  if (!service)
    return;

  if (service->statistics)
    free(service->statistics);

  free(service);

  return;
}

int httpd_enable_service(hservice_t *service)
{
  int ret;

  ret = service->status;
  service->status = NHTTPD_SERVICE_UP;

  return ret;
}

int httpd_disable_service(hservice_t *service)
{
  int ret;
  
  ret = service->status;
  service->status = NHTTPD_SERVICE_DOWN;

  return ret;
}

hservice_t *
httpd_find_service(const char *context)
{
  hservice_t *cur;

  for (cur = _httpd_services_head; cur; cur = cur->next)
  {
    if (!strcmp(cur->ctx, context))
      return cur;
  }

  return _httpd_services_default;
}

void
httpd_response_set_content_type(httpd_conn_t * res, const char *content_type)
{
  strncpy(res->content_type, content_type, 25);

  return;
}

herror_t
httpd_send_header(httpd_conn_t * res, int code, const char *text)
{
  struct tm stm;
  time_t nw;
  char buffer[255];
  char header[1024];
  hpair_t *cur;
  herror_t status;

  /* set status code */
  sprintf(header, "HTTP/1.1 %d %s\r\n", code, text);

  /* set date */
  nw = time(NULL);
  localtime_r(&nw, &stm);
  strftime(buffer, 255, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", &stm);
  strcat(header, buffer);

  /* set content-type */
  /* 
   * if (res->content_type[0] == '\0') { strcat(header, "Content-Type:
   * text/html\r\n"); } else { sprintf(buffer, "Content-Type: %s\r\n",
   * res->content_type); strcat(header, buffer); }
   */

  /* set server name */
  strcat(header, "Server: Nano HTTPD library\r\n");

  /* set _httpd_connection status */
  /* strcat (header, "Connection: close\r\n"); */

  /* add pairs */
  for (cur = res->header; cur; cur = cur->next)
  {
    sprintf(buffer, "%s: %s\r\n", cur->key, cur->value);
    strcat(header, buffer);
  }

  /* set end of header */
  strcat(header, "\r\n");

  /* send header */
  if ((status = hsocket_send(res->sock, header, strlen(header))) != H_OK)
    return status;

  res->out = http_output_stream_new(res->sock, res->header);
  return H_OK;
}

herror_t
httpd_send_internal_error(httpd_conn_t * conn, const char *errmsg)
{
  const char *template1 =
    "<html>"
      "<head>"
      "</head>"
      "<body>"
        "<h3>Error!</h3>"
	"<hr/>"
        "<div>Message: '%s'</div>"
      "</body>"
    "</html>";

  char buffer[4096];
  char buflen[5];

  sprintf(buffer, template1, errmsg);
  snprintf(buflen, 5, "%d", strlen(buffer));

  httpd_set_header(conn, HEADER_CONTENT_LENGTH, buflen);
  httpd_send_header(conn, 500, HTTP_STATUS_500_REASON_PHRASE);

  return http_output_stream_write_string(conn->out, buffer);
}

static void
_httpd_request_print(struct hrequest_t * req)
{
  hpair_t *pair;

  log_verbose1("++++++ Request +++++++++");
  log_verbose2(" Method : '%s'",
               (req->method == HTTP_REQUEST_POST) ? "POST" : "GET");
  log_verbose2(" Path   : '%s'", req->path);
  log_verbose2(" Spec   : '%s'",
               (req->version == HTTP_1_0) ? "HTTP/1.0" : "HTTP/1.1");
  log_verbose1(" Parsed query string :");

  for (pair = req->query; pair; pair = pair->next)
    log_verbose3(" %s = '%s'", pair->key, pair->value);

  log_verbose1(" Parsed header :");
  for (pair = req->header; pair; pair = pair->next)
    log_verbose3(" %s = '%s'", pair->key, pair->value);

  log_verbose1("++++++++++++++++++++++++");

  return;
}

httpd_conn_t *
httpd_new(struct hsocket_t * sock)
{
  httpd_conn_t *conn;

  if (!(conn = (httpd_conn_t *) malloc(sizeof(httpd_conn_t))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }
  conn->sock = sock;
  conn->out = NULL;
  conn->content_type[0] = '\0';
  conn->header = NULL;

  return conn;
}

void
httpd_free(httpd_conn_t * conn)
{
  if (!conn)
    return;

  if (conn->out)
    http_output_stream_free(conn->out);

  if (conn->header)
    hpairnode_free_deep(conn->header);

  free(conn);

  return;
}

static int
_httpd_decode_authorization(const char *value, char **user, char **pass)
{
  unsigned char *tmp, *tmp2;
  size_t len;

  len = strlen(value) * 2;
  if (!(tmp = (char *) calloc(1, len)))
  {
    log_error2("calloc failed (%s)", strerror(errno));
    return -1;
  }

  value = strstr(value, " ") + 1;

  log_verbose2("Authorization (base64) = \"%s\"", value);

  base64_decode_string(value, tmp);

  log_verbose2("Authorization (ascii) = \"%s\"", tmp);

  if ((tmp2 = strstr(tmp, ":")))
  {
    *tmp2++ = '\0';
    *pass = strdup(tmp2);
  }
  else
  {
    *pass = strdup("");
  }
  *user = strdup(tmp);

  free(tmp);

  return 0;
}

static int
_httpd_authenticate_request(struct hrequest_t * req, httpd_auth auth)
{
  char *user, *pass;
  char *authorization;
  int ret;

  if (!auth)
    return 1;

  if (!(authorization = hpairnode_get_ignore_case(req->header, HEADER_AUTHORIZATION)))
  {
    log_debug2("\"%s\" header not set", HEADER_AUTHORIZATION);
    return 0;
  }

  if (_httpd_decode_authorization(authorization, &user, &pass))
  {
    log_error1("httpd_base64_decode_failed");
    return 0;
  }

  if ((ret = auth(req, user, pass)))
    log_debug2("Access granted for user=\"%s\"", user);
  else
    log_info2("Authentication failed for user=\"%s\"", user);

  free(user);
  free(pass);

  return ret;
}

#ifdef WIN32
static unsigned _stdcall
httpd_session_main(void *data)
#else
static void *
httpd_session_main(void *data)
#endif
{
  struct hrequest_t *req;
  conndata_t *conn;
  httpd_conn_t *rconn;
  hservice_t *service;
  herror_t status;
  struct timeval start, end, duration;
  int done;

  if (gettimeofday(&start, NULL) == -1)
    log_error2("gettimeofday failed (%s)", strerror(errno));

  conn = (conndata_t *) data;

  log_verbose2("starting new httpd session on socket %d", conn->sock);

  rconn = httpd_new(&(conn->sock));

  done = 0;
  while (!done)
  {
    log_verbose3("starting HTTP request on socket %d (%p)", conn->sock, conn->sock.sock);

    if ((status = hrequest_new_from_socket(&(conn->sock), &req)) != H_OK)
    {
      int code;

      switch ((code = herror_code(status)))
      {
      case HSOCKET_ERROR_SSLCLOSE:
      case HSOCKET_ERROR_RECEIVE:
        log_error2("hrequest_new_from_socket failed (%s)", herror_message(status));
        break;
      default:
        httpd_send_internal_error(rconn, herror_message(status));
        break;
      }
      herror_release(status);
      done = 1;
    }
    else
    {
      char *conn_str;

      _httpd_request_print(req);

      conn_str = hpairnode_get_ignore_case(req->header, HEADER_CONNECTION);
      if (conn_str && strncasecmp(conn_str, "close", 6) == 0)
        done = 1;

      if (!done)
        done = req->version == HTTP_1_0 ? 1 : 0;

      if ((service = httpd_find_service(req->path)))
      {
        log_verbose3("service '%s' for '%s' found", service->ctx, req->path);

	if (service->status == NHTTPD_SERVICE_UP)
	{
          pthread_rwlock_wrlock(&(service->statistics->lock));
          service->statistics->requests++;
          pthread_rwlock_unlock(&(service->statistics->lock));

          if (_httpd_authenticate_request(req, service->auth))
          {
            if (service->func != NULL)
            {
              service->func(rconn, req);

              if (gettimeofday(&end, NULL) == -1)
                log_error2("gettimeofday failed (%s)", strerror(errno));
              timersub(&end, &start, &duration);

              pthread_rwlock_wrlock(&(service->statistics->lock));
              service->statistics->bytes_received += rconn->sock->bytes_received;
              service->statistics->bytes_transmitted += rconn->sock->bytes_transmitted;
              timeradd(&(service->statistics->time), &duration, &(service->statistics->time));
              pthread_rwlock_unlock(&(service->statistics->lock));

              if (rconn->out && rconn->out->type == HTTP_TRANSFER_CONNECTION_CLOSE)
              {
                log_verbose1("Connection close requested");
                done = 1;
              }
            }
            else
            {
              char buffer[256];

              sprintf(buffer, "service '%s' not registered properly (service function is NULL)", req->path);
              log_verbose1(buffer);
              httpd_send_internal_error(rconn, buffer);
            }
	  }
          else
          {
            char *template =
              "<html>"
                "<head>"
                  "<title>Unauthorized</title>"
                "</head>"
                "<body>"
                  "<h1>Unauthorized request logged</h1>"
                "</body>"
              "</html>";

            httpd_set_header(rconn, HEADER_WWW_AUTHENTICATE, "Basic realm=\"nanoHTTP\"");
            httpd_send_header(rconn, 401, HTTP_STATUS_401_REASON_PHRASE);
            http_output_stream_write_string(rconn->out, template);
            done = 1;
          }
        }
        else
        {
          char buffer[256];

          sprintf(buffer, "service for '%s' is disabled", req->path);
          log_verbose1(buffer);
          httpd_send_internal_error(rconn, buffer);
        }
      }
      else
      {
        char buffer[256];
        sprintf(buffer, "no service for '%s' found", req->path);
        log_verbose1(buffer);
        httpd_send_internal_error(rconn, buffer);
        done = 1;
      }
      hrequest_free(req);
    }
  }

  httpd_free(rconn);

  hsocket_close(&(conn->sock));

#ifdef WIN32
  CloseHandle((HANDLE) conn->tid);
#else
  pthread_attr_destroy(&(conn->attr));
#endif

  conn->flag = CONNECTION_FREE;

#ifdef WIN32
  _endthread();
  return 0;
#else
  /* pthread_exits automagically */
  return NULL;
#endif
}

int
httpd_set_header(httpd_conn_t * conn, const char *key, const char *value)
{
  hpair_t *p;

  if (conn == NULL)
  {
    log_warn1("Connection object is NULL");
    return 0;
  }

  for (p = conn->header; p; p = p->next)
  {
    if (p->key && !strcmp(p->key, key))
    {
      free(p->value);
      p->value = strdup(value);
      return 1;
    }
  }

  conn->header = hpairnode_new(key, value, conn->header);

  return 0;
}

void
httpd_set_headers(httpd_conn_t * conn, hpair_t * header)
{
  while (header)
  {
    httpd_set_header(conn, header->key, header->value);
    header = header->next;
  }
  return;
}

int
httpd_add_header(httpd_conn_t * conn, const char *key, const char *value)
{
  if (!conn)
  {
    log_warn1("Connection object is NULL");
    return 0;
  }

  conn->header = hpairnode_new(key, value, conn->header);

  return 1;
}

void
httpd_add_headers(httpd_conn_t * conn, const hpair_t * values)
{
  if (!conn)
  {
    log_warn1("Connection object is NULL");
    return;
  }

  while (values)
  {
    httpd_add_header(conn, values->key, values->value);
    values = values->next;
  }
  return;
}

/*
 * -----------------------------------------------------
 * FUNCTION: _httpd_register_signal_handler
 * -----------------------------------------------------
 */
static void
_httpd_register_signal_handler(void)
{
  log_verbose2("registering termination signal handler (SIGNAL:%d)",
               _httpd_terminate_signal);
#ifdef WIN32
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE) _httpd_term, TRUE) == FALSE)
  {
    log_error1("Unable to install console event handler!");
  }

#else
  signal(_httpd_terminate_signal, _httpd_term);
#endif

  return;
}

static conndata_t *
_httpd_wait_for_empty_conn(void)
{
  int i;

#ifdef WIN32
  WaitForSingleObject(_httpd_connection_lock, INFINITE);
#else
  pthread_mutex_lock(&_httpd_connection_lock);
#endif

  for (i = 0;; i++)
  {
    if (!_httpd_run)
    {
#ifdef WIN32
      ReleaseMutex(_httpd_connection_lock);
#else
      pthread_mutex_unlock(&_httpd_connection_lock);
#endif
      return NULL;
    }

    if (i >= _httpd_max_connections)
    {
      _httpd_sys_sleep(1);
      i = -1;
    }
    else if (_httpd_connection[i].flag == CONNECTION_FREE)
    {
      _httpd_connection[i].flag = CONNECTION_IN_USE;
      break;
    }
  }

#ifdef WIN32
  ReleaseMutex(_httpd_connection_lock);
#else
  pthread_mutex_unlock(&_httpd_connection_lock);
#endif

  return &_httpd_connection[i];
}

static void
_httpd_start_thread(conndata_t * conn)
{
  int err;

#ifdef WIN32
  conn->tid =
    (HANDLE) _beginthreadex(NULL, 65535, httpd_session_main, conn, 0, &err);
#else
  pthread_attr_init(&(conn->attr));

#ifdef PTHREAD_CREATE_DETACHED
  pthread_attr_setdetachstate(&(conn->attr), PTHREAD_CREATE_DETACHED);
#endif

  pthread_sigmask(SIG_BLOCK, &thrsigset, NULL);
  if ((err =
       pthread_create(&(conn->tid), &(conn->attr), httpd_session_main, conn)))
    log_error2("pthread_create failed (%s)", strerror(err));
#endif

  return;
}

herror_t
httpd_run(void)
{
  struct timeval timeout;
  conndata_t *conn;
  herror_t err;
  fd_set fds;

  log_verbose1("starting run routine");

#ifndef WIN32
  sigemptyset(&thrsigset);
  sigaddset(&thrsigset, SIGALRM);
#endif

  _httpd_register_signal_handler();

  if ((err = hsocket_listen(&_httpd_socket)) != H_OK)
  {
    log_error2("hsocket_listen failed (%s)", herror_message(err));
    return err;
  }

  while (_httpd_run)
  {
    conn = _httpd_wait_for_empty_conn();
    if (!_httpd_run)
      break;

    /* Wait for a socket to accept */
    while (_httpd_run)
    {

      /* set struct timeval to the proper timeout */
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      /* zero and set file descriptior */
      FD_ZERO(&fds);
      FD_SET(_httpd_socket.sock, &fds);

      /* select socket descriptor */
      switch (select(_httpd_socket.sock + 1, &fds, NULL, NULL, &timeout))
      {
      case 0:
        /* descriptor is not ready */
        continue;
      case -1:
        /* got a signal? */
        continue;
      default:
        /* no nothing */
        break;
      }
      if (FD_ISSET(_httpd_socket.sock, &fds))
      {
        break;
      }
    }

    /* check signal status */
    if (!_httpd_run)
      break;

    if ((err = hsocket_accept(&_httpd_socket, &(conn->sock))) != H_OK)
    {
      log_error2("hsocket_accept failed (%s)", herror_message(err));

      hsocket_close(&(conn->sock));

      continue;
    }

    _httpd_start_thread(conn);
  }

  return 0;
}

void
httpd_destroy(void)
{
  hservice_t *tmp, *cur = _httpd_services_head;

  while (cur != NULL)
  {
    tmp = cur->next;
    hservice_free(cur);
    cur = tmp;
  }

  hsocket_module_destroy();

  free(_httpd_connection);

  return;
}

unsigned char *
httpd_get_postdata(httpd_conn_t * conn, struct hrequest_t * req, long *received, long max)
{
  char *content_length_str;
  long content_length = 0;
  unsigned char *postdata = NULL;

  if (req->method == HTTP_REQUEST_POST)
  {

    content_length_str =
      hpairnode_get_ignore_case(req->header, HEADER_CONTENT_LENGTH);

    if (content_length_str != NULL)
      content_length = atol(content_length_str);

  }
  else
  {
    log_warn1("Not a POST method");
    return NULL;
  }

  if (content_length > max && max != -1)
    return NULL;

  if (content_length == 0)
  {
    *received = 0;
    if (!(postdata = (char *) malloc(1)))
    {

      log_error2("malloc failed (%s)", strerror(errno));
      return NULL;
    }
    postdata[0] = '\0';
    return postdata;
  }
  if (!(postdata = (unsigned char *) malloc(content_length + 1)))
  {
    log_error2("malloc failed (%)", strerror(errno));
    return NULL;
  }
  if (http_input_stream_read(req->in, postdata, (int) content_length) > 0)
  {
    *received = content_length;
    postdata[content_length] = '\0';
    return postdata;
  }
  free(postdata);
  return NULL;
}


/*
  MIME support httpd_mime_* function set
*/

static void
_httpd_mime_get_boundary(httpd_conn_t * conn, char *dest)
{
  sprintf(dest, "---=.Part_NH_%p", conn);
  log_verbose2("boundary= \"%s\"", dest);

  return;
}


/**
  Begin MIME multipart/related POST 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_send_header(httpd_conn_t * conn, const char *related_start,
                       const char *related_start_info,
                       const char *related_type, int code, const char *text)
{
  char buffer[300];
  char temp[250];
  char boundary[250];

  /* Set Content-type Set multipart/related parameter type=..; start=.. ;
     start-info= ..; boundary=... using sprintf instead of snprintf because 
     visual c does not support snprintf */

  sprintf(buffer, "multipart/related;");

  if (related_type)
  {
    snprintf(temp, 75, " type=\"%s\";", related_type);
    strcat(buffer, temp);
  }

  if (related_start)
  {
    snprintf(temp, 250, " start=\"%s\";", related_start);
    strcat(buffer, temp);
  }

  if (related_start_info)
  {
    snprintf(temp, 250, " start-info=\"%s\";", related_start_info);
    strcat(buffer, temp);
  }

  _httpd_mime_get_boundary(conn, boundary);
  snprintf(temp, 250, " boundary=\"%s\"", boundary);
  strcat(buffer, temp);

  httpd_set_header(conn, HEADER_CONTENT_TYPE, buffer);

  return httpd_send_header(conn, code, text);
}


/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_next(httpd_conn_t * conn, const char *content_id,
                const char *content_type, const char *transfer_encoding)
{
  herror_t status;
  char buffer[512];
  char boundary[75];

  /* Get the boundary string */
  _httpd_mime_get_boundary(conn, boundary);
  sprintf(buffer, "\r\n--%s\r\n", boundary);

  /* Send boundary */
  status =
    http_output_stream_write(conn->out, buffer, strlen(buffer));

  if (status != H_OK)
    return status;

  /* Send Content header */
  sprintf(buffer, "%s: %s\r\n%s: %s\r\n%s: %s\r\n\r\n",
          HEADER_CONTENT_TYPE, content_type ? content_type : "text/plain",
          HEADER_CONTENT_TRANSFER_ENCODING,
          transfer_encoding ? transfer_encoding : "binary",
          HEADER_CONTENT_ID,
          content_id ? content_id : "<content-id-not-set>");

  status =
    http_output_stream_write(conn->out, buffer, strlen(buffer));

  return status;
}

/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_send_file(httpd_conn_t * conn, const char *content_id,
                     const char *content_type, const char *transfer_encoding,
                     const char *filename)
{
  unsigned char buffer[MAX_FILE_BUFFER_SIZE];
  herror_t status;
  FILE *fd;
  size_t size;

  if ((fd = fopen(filename, "rb")) == NULL)
    return herror_new("httpd_mime_send_file", FILE_ERROR_OPEN,
                      "Can not open file '%d'", filename);

  status = httpd_mime_next(conn, content_id, content_type, transfer_encoding);
  if (status != H_OK)
  {
    fclose(fd);
    return status;
  }

  while (!feof(fd))
  {
    size = fread(buffer, 1, MAX_FILE_BUFFER_SIZE, fd);
    if (size == -1)
    {
      fclose(fd);
      return herror_new("httpd_mime_send_file", FILE_ERROR_READ,
                        "Can not read from file '%d'", filename);
    }

    if ((status = http_output_stream_write(conn->out, buffer, size)) != H_OK)
    {
      fclose(fd);
      return status;
    }
  }

  fclose(fd);
  return H_OK;
}

/**
  Finish MIME request 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_end(httpd_conn_t * conn)
{
  herror_t status;
  char buffer[512];
  char boundary[75];

  /* Get the boundary string */
  _httpd_mime_get_boundary(conn, boundary);
  sprintf(buffer, "\r\n--%s--\r\n\r\n", boundary);

  /* Send boundary */
  status =
    http_output_stream_write(conn->out, buffer, strlen(buffer));

  if (status != H_OK)
    return status;

  /* Flush put stream */
  status = http_output_stream_flush(conn->out);

  return status;
}
