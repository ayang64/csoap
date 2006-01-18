/******************************************************************
*  $Id: nanohttp-server.c,v 1.44 2006/01/18 14:53:13 mrcsys Exp $
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
#include <nanohttp/nanohttp-server.h>
#include <nanohttp/nanohttp-ssl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32

/* According to POSIX 1003.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

#ifdef HAVE_SOCKET
#include <sys/socket.h>
#endif

#else

#include <process.h>

#endif

#ifdef MEM_DEBUG
#include <utils/alloc.h>
#endif


typedef struct _conndata
{
  hsocket_t sock;
#ifdef WIN32
  HANDLE tid;
#else
  pthread_t tid;
  pthread_attr_t attr;
#endif
  time_t atime;
}
conndata_t;

/*
 * -----------------------------------------------------
 * nano httpd
 * internally globals
 * -----------------------------------------------------
 */
static int _httpd_port = 10000;
static int _httpd_max_connections = 20;
#ifdef WIN32
static int _httpd_max_idle = 120;
#endif
static hsocket_t _httpd_socket;
static hservice_t *_httpd_services_head = NULL;
static hservice_t *_httpd_services_tail = NULL;
static int _httpd_run = 1;
#ifdef WIN32
static DWORD _httpd_terminate_signal = CTRL_C_EVENT;
#else
static int _httpd_terminate_signal = SIGINT;
#endif
static conndata_t *_httpd_connection;


#ifdef WIN32
static void WSAReaper (void *x);
#else
sigset_t thrsigset;
#endif

#ifdef HAVE_SSL
/*extern SSL_CTX *SSLctx;*/
#endif


/*
 * -----------------------------------------------------
 * FUNCTION: httpd_init
 * NOTE: This will be called from soap_server_init_args()
 * -----------------------------------------------------
 */
herror_t
httpd_init (int argc, char *argv[])
{
  int i;
  herror_t status;

  hoption_init_args (argc, argv);

  status = hsocket_module_init ();
  if (status != H_OK)
    return status;

  /* write argument information */
  log_verbose1 ("Arguments:");
  for (i = 0; i < argc; i++)
    log_verbose3 ("argv[%i] = '%s'", i, SAVE_STR (argv[i]));

  /* initialize from arguments */
  for (i = 0; i < argc; i++)
  {
    if (!strcmp (argv[i], NHTTPD_ARG_PORT) && i < argc - 1)
    {
      _httpd_port = atoi (argv[i + 1]);
    }
    else if (!strcmp (argv[i], NHTTPD_ARG_TERMSIG) && i < argc - 1)
    {
      _httpd_terminate_signal = atoi (argv[i + 1]);
    }
    else if (!strcmp (argv[i], NHTTPD_ARG_MAXCONN) && i < argc - 1)
    {
      _httpd_max_connections = atoi (argv[i + 1]);
    }
  }

  log_verbose2 ("socket bind to port '%d'", _httpd_port);

  /* init built-in services */

  /* httpd_register("/httpd/list", service_list); */

  _httpd_connection = calloc (_httpd_max_connections, sizeof (conndata_t));
  for (i = 0; i < _httpd_max_connections; i++)
  {
    memset ((char *) &_httpd_connection[i], 0, sizeof (_httpd_connection[i]));
  }

#ifdef WIN32
  /* 
     if (_beginthread (WSAReaper, 0, NULL) == -1) { log_error1 ("Winsock
     reaper thread failed to start"); return herror_new("httpd_init",
     THREAD_BEGIN_ERROR, "_beginthread() failed while starting WSAReaper"); } 
   */
#endif

#ifdef HAVE_SSL
  start_ssl();
#endif

  /* create socket */
  status = hsocket_init (&_httpd_socket);
  if (status != H_OK)
  {
    return status;
  }
  status = hsocket_bind (&_httpd_socket, _httpd_port);

  return status;
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_register
 * -----------------------------------------------------
 */

int
httpd_register (const char *ctx, httpd_service func)
{
  hservice_t *service;

  service = (hservice_t *) malloc (sizeof (hservice_t));
  service->next = NULL;
  service->func = func;
  strcpy (service->ctx, ctx);

  log_verbose3 ("register service:t(%p):%s", service, SAVE_STR (ctx));
  if (_httpd_services_head == NULL)
  {
    _httpd_services_head = _httpd_services_tail = service;
  }
  else
  {
    _httpd_services_tail->next = service;
    _httpd_services_tail = service;
  }

  return 1;
}


/*
 * -----------------------------------------------------
 * FUNCTION: httpd_services
 * -----------------------------------------------------
 */
hservice_t *
httpd_services ()
{
  return _httpd_services_head;
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_services
 * -----------------------------------------------------
 */
static void
hservice_free (hservice_t * service)
{
  free (service);
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_find_service
 * -----------------------------------------------------
 */
static hservice_t *
httpd_find_service (const char *ctx)
{
  hservice_t *cur = _httpd_services_head;

  while (cur != NULL)
  {
    if (!strcmp (cur->ctx, ctx))
    {
      return cur;
    }
    cur = cur->next;
  }

  return NULL;
}


/*
 * -----------------------------------------------------
 * FUNCTION: httpd_response_set_content_type
 * -----------------------------------------------------
 */
void
httpd_response_set_content_type (httpd_conn_t * res, const char *content_type)
{
  strncpy (res->content_type, content_type, 25);
}


/*
 * -----------------------------------------------------
 * FUNCTION: httpd_response_send_header
 * -----------------------------------------------------
 */
herror_t
httpd_send_header (httpd_conn_t * res, int code, const char *text)
{
  struct tm stm;
  time_t nw;
  char buffer[255];
  char header[1024];
  hpair_t *cur;
  herror_t status;

  /* set status code */
  sprintf (header, "HTTP/1.1 %d %s\r\n", code, text);

  /* set date */
  nw = time (NULL);
  localtime_r (&nw, &stm);
  strftime (buffer, 255, "Date: %a, %d %b %Y %T GMT", &stm);
  strcat (header, buffer);
  strcat (header, "\r\n");

  /* set content-type */
  /* 
   * if (res->content_type[0] == '\0') { strcat(header, "Content-Type:
   * text/html\r\n"); } else { sprintf(buffer, "Content-Type: %s\r\n",
   * res->content_type); strcat(header, buffer); }
   */

  /* set server name */
  strcat (header, "Server: Nano HTTPD library\r\n");

  /* set _httpd_connection status */
  // strcat (header, "Connection: close\r\n");

  /* add pairs */
  cur = res->header;
  while (cur != NULL)
  {
    sprintf (buffer, "%s: %s\r\n", cur->key, cur->value);
    strcat (header, buffer);
    cur = cur->next;
  }

  /* set end of header */
  strcat (header, "\r\n");

  /* send header */
  status = hsocket_nsend (res->sock, header, strlen (header));
  if (status != H_OK)
    return status;

  res->out = http_output_stream_new (res->sock, res->header);
  return H_OK;
}


herror_t
httpd_send_internal_error (httpd_conn_t * conn, const char *errmsg)
{
  const char *template1 =
    "<html><body><h3>Error!</h3><hr> Message: '%s' </body></html>\r\n";

  char buffer[4064];
  char buflen[5];
  sprintf (buffer, template1, errmsg);
#ifdef WIN32
#define snprintf(buffer, num, s1, s2) sprintf(buffer, s1,s2)
#endif
  snprintf (buflen, 5, "%d", strlen (buffer));
  httpd_set_header (conn, HEADER_CONTENT_LENGTH, buflen);
  httpd_send_header (conn, 500, "INTERNAL");
  return hsocket_nsend (conn->sock, buffer, strlen (buffer));
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_request_print
 * -----------------------------------------------------
 */
static void
httpd_request_print (hrequest_t * req)
{
  hpair_t *pair;

  log_verbose1 ("++++++ Request +++++++++");
  log_verbose2 (" Method : '%s'",
                (req->method == HTTP_REQUEST_POST) ? "POST" : "GET");
  log_verbose2 (" Path   : '%s'", req->path);
  log_verbose2 (" Spec   : '%s'",
                (req->version == HTTP_1_0) ? "HTTP/1.0" : "HTTP/1.1");
  log_verbose1 (" Parsed query string :");

  pair = req->query;
  while (pair != NULL)
  {
    log_verbose3 (" %s = '%s'", pair->key, pair->value);
    pair = pair->next;
  }
  log_verbose1 (" Parsed header :");
  pair = req->header;
  while (pair != NULL)
  {
    log_verbose3 (" %s = '%s'", pair->key, pair->value);
    pair = pair->next;
  }
  log_verbose1 ("++++++++++++++++++++++++");

}


httpd_conn_t *
httpd_new (hsocket_t sock)
{
  httpd_conn_t *conn = (httpd_conn_t *) malloc (sizeof (httpd_conn_t));
  conn->sock = sock;
  conn->out = NULL;
  conn->content_type[0] = '\0';
  conn->header = NULL;

  return conn;
}


void
httpd_free (httpd_conn_t * conn)
{
  if (conn->out != NULL)
    http_output_stream_free (conn->out);

  if (conn->header != NULL)
    hpairnode_free_deep (conn->header);

  free (conn);
}

void
do_req_timeout (int signum)
{
/*
    struct sigaction req_timeout;
    memset(&req_timeout, 0, sizeof(&req_timeout));
    req_timeout.sa_handler=SIG_IGN;
    sigaction(SIGALRM, &req_timeout, NULL);
*/

  // XXX this is not real pretty, is there a better way?
  log_verbose1 ("Thread timeout.");
#ifdef WIN32
  _endthread ();
#else
  pthread_exit (0);
#endif
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_session_main
 * -----------------------------------------------------
 */
#ifdef WIN32
static unsigned _stdcall
httpd_session_main (void *data)
#else
static void *
httpd_session_main (void *data)
#endif
{
  conndata_t *conn = (conndata_t *) data;
  const char *msg = "SESSION 1.0\n";
  int len = strlen (msg);
  int done = 0;
  char buffer[256];             /* temp buffer for recv() */
  char header[4064];            /* received header */
  hrequest_t *req = NULL;       /* only for test */
  httpd_conn_t *rconn = NULL;
  hservice_t *service = NULL;
  herror_t status;

  header[0] = '\0';
  len = 0;

  log_verbose1 ("starting httpd_session_main()");
#ifdef HAVE_SSL
  if (!conn->sock.sslCtx)
  {
    log_verbose1 ("Using HTTP");
  }
  else
  {
    log_verbose1 ("Using HTTPS");
    conn->sock.ssl = init_ssl (conn->sock.sslCtx, conn->sock.sock, SSL_SERVER);
    hsocket_block (conn->sock, 0);
    if (conn->sock.ssl == NULL)
    {
      done = 1;
    }
  }
#endif
  conn->atime = time ((time_t) 0);
  /* call the service */
/*  req = hrequest_new_from_buffer (header);*/

  while (!done)
  {
    log_verbose1 ("starting HTTP request");
    rconn = httpd_new (conn->sock);

    status = hrequest_new_from_socket (conn->sock, &req);

    if (status != H_OK)
    {
      if (herror_code (status) != HSOCKET_ERROR_SSLCLOSE)
      {
        httpd_send_internal_error (rconn, herror_message (status)       /* "Request
                                                                           parse
                                                                           error!" */ );
        herror_release (status);
      }
      done = 1;
    }
    else
    {
      char *conn_str =
        hpairnode_get_ignore_case (req->header, HEADER_CONNECTION);
#ifdef WIN32
#define strncasecmp(s1, s2, num) strncmp(s1, s2, num)
#endif
      if (conn_str && strncasecmp (conn_str, "close", 5) == 0)
      {
        done = 1;
      }
      if (!done)
      {
        done = req->version == HTTP_1_0 ? 1 : 0;
      }
      httpd_request_print (req);


      service = httpd_find_service (req->path);
      if (service != NULL)
      {
        log_verbose2 ("service '%s' found", req->path);
        if (service->func != NULL)
        {
          service->func (rconn, req);
          if (rconn->out
              && rconn->out->type == HTTP_TRANSFER_CONNECTION_CLOSE)
          {
            log_verbose1 ("Connection close requested");
            done = 1;
          }
        }
        else
        {
          sprintf (buffer,
                   "service '%s' not registered properly (func == NULL)",
                   req->path);
          log_verbose1 (buffer);
          httpd_send_internal_error (rconn, buffer);
        }
      }
      else
      {
        sprintf (buffer, "service '%s' not found", req->path);
        log_verbose1 (buffer);
        httpd_send_internal_error (rconn, buffer);
      }

      /* httpd_response_free(res); */
      /* hrequest_free (req); */
    }
  }

  hsocket_close (conn->sock);
  log_verbose1 ("Marking connection as available");
  conn->sock.sock = 0;
  hrequest_free (req);
  httpd_free (rconn);
#ifdef WIN32
  CloseHandle ((HANDLE) conn->tid);
  _endthread ();
  return 0;
#else
  pthread_exit (NULL);
  return service;
#endif
}

int
httpd_set_header (httpd_conn_t * conn, const char *key, const char *value)
{
  hpair_t *p;

  if (conn == NULL)
  {
    log_warn1 ("Connection object is NULL");
    return 0;
  }
  p = conn->header;
  while (p != NULL)
  {
    if (p->key != NULL)
    {
      if (!strcmp (p->key, key))
      {
        free (p->value);
        p->value = (char *) malloc (strlen (value) + 1);
        strcpy (p->value, value);
        return 1;
      }
    }
    p = p->next;
  }

  conn->header = hpairnode_new (key, value, conn->header);
  return 0;
}


void
httpd_set_headers (httpd_conn_t * conn, hpair_t * header)
{
  while (header)
  {
    httpd_set_header (conn, header->key, header->value);
    header = header->next;
  }
}

/*
 * -----------------------------------------------------
 * FUNCTION: httpd_term
 * -----------------------------------------------------
 */
#ifdef WIN32
BOOL WINAPI
httpd_term (DWORD sig)
{
  // log_debug2 ("Got signal %d", sig);
  if (sig == _httpd_terminate_signal)
    _httpd_run = 0;
  return TRUE;
}

#else

void
httpd_term (int sig)
{
  log_debug2 ("Got signal %d", sig);
  if (sig == _httpd_terminate_signal)
    _httpd_run = 0;
}

#endif


/*
 * -----------------------------------------------------
 * FUNCTION: _httpd_register_signal_handler
 * -----------------------------------------------------
 */
static void
_httpd_register_signal_handler ()
{
  log_verbose2 ("registering termination signal handler (SIGNAL:%d)",
                _httpd_terminate_signal);
#ifdef WIN32
  if (SetConsoleCtrlHandler ((PHANDLER_ROUTINE) httpd_term, TRUE) == FALSE)
  {
    log_error1 ("Unable to install console event handler!");
  }

#else
  signal (_httpd_terminate_signal, httpd_term);
#endif
}




/*--------------------------------------------------
FUNCTION: _httpd_wait_for_empty_conn
----------------------------------------------------*/
static conndata_t *
_httpd_wait_for_empty_conn ()
{
  int i;
  for (i = 0;; i++)
  {
    if (!_httpd_run)
      return NULL;

    if (i >= _httpd_max_connections)
    {
      system_sleep (1);
      i = 0;
    }
    else if (_httpd_connection[i].sock.sock == 0)
    {
      break;
    }
  }

  return &_httpd_connection[i];
}

/*
 * -----------------------------------------------------
 * FUNCTION: _httpd_start_thread
 * -----------------------------------------------------
 */
static void
_httpd_start_thread (conndata_t * conn)
{
  int err;

#ifdef WIN32
  conn->tid =
    (HANDLE) _beginthreadex (NULL, 65535, httpd_session_main, conn, 0, &err);
#else
  pthread_attr_init (&(conn->attr));
#ifdef PTHREAD_CREATE_DETACHED
  pthread_attr_setdetachstate (&(conn->attr), PTHREAD_CREATE_DETACHED);
#endif
  pthread_sigmask (SIG_BLOCK, &thrsigset, NULL);
  err =
    pthread_create (&(conn->tid), &(conn->attr), httpd_session_main, conn);
  if (err)
  {
    log_error2 ("Error creating thread: ('%d')", err);
  }
#endif
}



/*
 * -----------------------------------------------------
 * FUNCTION: httpd_run
 * -----------------------------------------------------
 */

herror_t
httpd_run ()
{
  herror_t err;
  conndata_t *conn;
  fd_set fds;
  struct timeval timeout;


  log_verbose1 ("starting run routine");

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
#ifdef WIN32
#else
  sigemptyset (&thrsigset);
  sigaddset (&thrsigset, SIGALRM);
#endif


  /* listen to port */
  err = hsocket_listen (_httpd_socket);
  if (err != H_OK)
  {
    log_error2 ("httpd_run(): '%d'", herror_message (err));
    return err;
  }
  log_verbose2 ("listening to port '%d'", _httpd_port);

  /* register signal handler */
  _httpd_register_signal_handler ();

  /* make the socket non blocking */
  err = hsocket_block (_httpd_socket, 0);
  if (err != H_OK)
  {
    log_error2 ("httpd_run(): '%s'", herror_message (err));
    return err;
  }


  while (_httpd_run)
  {
    /* Get an empty connection struct */
    conn = _httpd_wait_for_empty_conn ();
    if (!_httpd_run)
      break;


    /* Wait for a socket to accept */
    while (_httpd_run)
    {

      /* set struct timeval to the proper timeout */
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      /* zero and set file descriptior */
      FD_ZERO (&fds);
      FD_SET (_httpd_socket.sock, &fds);

      /* select socket descriptor */
      switch (select (_httpd_socket.sock + 1, &fds, NULL, NULL, &timeout))
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
      if (FD_ISSET (_httpd_socket.sock, &fds))
      {
        break;
      }
    }

    /* check signal status */
    if (!_httpd_run)
      break;

    /* Accept a socket */
    err = hsocket_accept (_httpd_socket, &(conn->sock));
    if (err != H_OK 
	/* TODO (#1#) is this check neccessary?
	   && herror_code (err) == SSL_ERROR_INIT*/
	)
    {
      hsocket_close (conn->sock);
      conn->sock.sock = -1;
#ifdef HAVE_SSL
      conn->sock.ssl = NULL;
#endif
      log_error1 (herror_message (err));
      continue;
    }
    else if (err != H_OK)
    {
      log_error2 ("Can not accept socket: %s", herror_message (err));
      return err;               /* this is hard core! */
    }

    /* Now start a thread */
    _httpd_start_thread (conn);
  }
  free (_httpd_connection);
  return 0;
}

void
httpd_destroy ()
{
  hservice_t *tmp, *cur = _httpd_services_head;

  while (cur != NULL)
  {
    tmp = cur->next;
    hservice_free (cur);
    cur = tmp;
  }

  hsocket_module_destroy ();
}

#ifdef WIN32

static void
WSAReaper (void *x)
{
  short int connections;
  short int i;
  char junk[10];
  int rc;
  time_t ctime;

  for (;;)
  {
    connections = 0;
    ctime = time ((time_t) 0);
    for (i = 0; i < _httpd_max_connections; i++)
    {
      if (_httpd_connection[i].tid == 0)
        continue;
      GetExitCodeThread ((HANDLE) _httpd_connection[i].tid, (PDWORD) & rc);
      if (rc != STILL_ACTIVE)
        continue;
      connections++;
      if ((ctime - _httpd_connection[i].atime < _httpd_max_idle) ||
          (_httpd_connection[i].atime == 0))
        continue;
      log_verbose3 ("Reaping socket %u from (runtime ~= %d seconds)",
                    _httpd_connection[i].sock,
                    ctime - _httpd_connection[i].atime);
      shutdown (_httpd_connection[i].sock.sock, 2);
      while (recv (_httpd_connection[i].sock.sock, junk, sizeof (junk), 0) >
             0)
      {
      };
      closesocket (_httpd_connection[i].sock.sock);
      _httpd_connection[i].sock.sock = 0;
      TerminateThread (_httpd_connection[i].tid, (DWORD) & rc);
      CloseHandle (_httpd_connection[i].tid);
      memset ((char *) &_httpd_connection[i], 0,
              sizeof (_httpd_connection[i]));
    }
    Sleep (100);
  }
  return;
}

#endif

unsigned char *
httpd_get_postdata (httpd_conn_t * conn, hrequest_t * req, long *received,
                    long max)
{
  char *content_length_str;
  long content_length = 0;
  unsigned char *postdata = NULL;

  if (req->method == HTTP_REQUEST_POST)
  {

    content_length_str =
      hpairnode_get_ignore_case (req->header, HEADER_CONTENT_LENGTH);

    if (content_length_str != NULL)
      content_length = atol (content_length_str);

  }
  else
  {
    log_warn1 ("Not a POST method");
    return NULL;
  }

  if (content_length > max && max != -1)
    return NULL;

  if (content_length == 0)
  {
    *received = 0;
    postdata = (char *) malloc (1);
    postdata[0] = '\0';
    return postdata;
  }
  postdata = (unsigned char *) malloc (content_length + 1);
  if (postdata == NULL)
  {
    log_error1 ("Not enough memory");
    return NULL;
  }
  if (http_input_stream_read (req->in, postdata, (int) content_length) > 0)
  {
    *received = content_length;
    postdata[content_length] = '\0';
    return postdata;
  }
  free (postdata);
  return NULL;
}




/*
  MIME support httpd_mime_* function set
*/

static void
_httpd_mime_get_boundary (httpd_conn_t * conn, char *dest)
{
  sprintf (dest, "---=.Part_NH_%p", conn);
  log_verbose2 ("boundary= \"%s\"", dest);
}


/**
  Begin MIME multipart/related POST 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_send_header (httpd_conn_t * conn,
                        const char *related_start,
                        const char *related_start_info,
                        const char *related_type, int code, const char *text)
{
  char buffer[300];
  char temp[250];
  char boundary[250];

  /* 
     Set Content-type Set multipart/related parameter type=..; start=.. ;
     start-info= ..; boundary=...

   */
  /* 
     using sprintf instead of snprintf because visual c does not support
     snprintf */

  sprintf (buffer, "multipart/related;");

  if (related_type)
  {
    snprintf (temp, 75, " type=\"%s\";", related_type);
    strcat (buffer, temp);
  }

  if (related_start)
  {
    snprintf (temp, 250, " start=\"%s\";", related_start);
    strcat (buffer, temp);
  }

  if (related_start_info)
  {
    snprintf (temp, 250, " start-info=\"%s\";", related_start_info);
    strcat (buffer, temp);
  }

  _httpd_mime_get_boundary (conn, boundary);
  snprintf (temp, 250, " boundary=\"%s\"", boundary);
  strcat (buffer, temp);

  httpd_set_header (conn, HEADER_CONTENT_TYPE, buffer);

  return httpd_send_header (conn, code, text);
}


/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_next (httpd_conn_t * conn,
                 const char *content_id,
                 const char *content_type, const char *transfer_encoding)
{
  herror_t status;
  char buffer[512];
  char boundary[75];

  /* Get the boundary string */
  _httpd_mime_get_boundary (conn, boundary);
  sprintf (buffer, "\r\n--%s\r\n", boundary);

  /* Send boundary */
  status = http_output_stream_write (conn->out,
                                     (const byte_t *) buffer,
                                     strlen (buffer));

  if (status != H_OK)
    return status;

  /* Send Content header */
  sprintf (buffer, "%s: %s\r\n%s: %s\r\n%s: %s\r\n\r\n",
           HEADER_CONTENT_TYPE, content_type ? content_type : "text/plain",
           HEADER_CONTENT_TRANSFER_ENCODING,
           transfer_encoding ? transfer_encoding : "binary",
           HEADER_CONTENT_ID,
           content_id ? content_id : "<content-id-not-set>");

  status = http_output_stream_write (conn->out,
                                     (const byte_t *) buffer,
                                     strlen (buffer));

  return status;
}

/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpd_mime_send_file (httpd_conn_t * conn,
                      const char *content_id,
                      const char *content_type,
                      const char *transfer_encoding, const char *filename)
{
  herror_t status;
  FILE *fd = fopen (filename, "rb");
  byte_t buffer[MAX_FILE_BUFFER_SIZE];
  size_t size;

  if (fd == NULL)
    return herror_new ("httpd_mime_send_file", FILE_ERROR_OPEN,
                       "Can not open file '%d'", filename);

  status =
    httpd_mime_next (conn, content_id, content_type, transfer_encoding);
  if (status != H_OK)
  {
    fclose (fd);
    return status;
  }

  while (!feof (fd))
  {
    size = fread (buffer, 1, MAX_FILE_BUFFER_SIZE, fd);
    if (size == -1)
    {
      fclose (fd);
      return herror_new ("httpd_mime_send_file", FILE_ERROR_READ,
                         "Can not read from file '%d'", filename);
    }

    status = http_output_stream_write (conn->out, buffer, size);
    if (status != H_OK)
    {
      fclose (fd);
      return status;
    }
  }

  fclose (fd);
  return H_OK;
}

/**
  Finish MIME request 
  Returns: H_OK  or error flag
*/
herror_t
httpd_mime_end (httpd_conn_t * conn)
{
  herror_t status;
  char buffer[512];
  char boundary[75];

  /* Get the boundary string */
  _httpd_mime_get_boundary (conn, boundary);
  sprintf (buffer, "\r\n--%s--\r\n\r\n", boundary);

  /* Send boundary */
  status = http_output_stream_write (conn->out,
                                     (const byte_t *) buffer,
                                     strlen (buffer));

  if (status != H_OK)
    return status;

  /* Flush put stream */
  status = http_output_stream_flush (conn->out);

  return status;
}
