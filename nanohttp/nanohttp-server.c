/******************************************************************
 *  $Id: nanohttp-server.c,v 1.4 2004/02/03 08:59:23 snowdrop Exp $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct tag_conndata
{
  hsocket_t sock;
}conndata_t;

/* ----------------------------------------------------- 
 nano httpd internally globals
----------------------------------------------------- */
static int _httpd_port = 10000;
static hsocket_t _httpd_socket;
static hservice_t *_httpd_services_head = NULL;
static hservice_t *_httpd_services_tail = NULL;


/* ----------------------------------------------------- 
 FUNCTION: httpd_init
----------------------------------------------------- */
int httpd_init(int argc, char *argv[])
{
  int i, status;

  /* write argument information */
  log_verbose1("Arguments:");  
  for (i=0;i<argc;i++)
    log_verbose3("argv[%i] = '%s'", i, SAVE_STR(argv[i]));

  /* initialize from arguments */
  for (i=0;i<argc;i++) {
    if (!strcmp(argv[i], NHTTPD_ARG_PORT) && i < argc-1) {
      _httpd_port = atoi(argv[i+1]);
    }
  }

  log_verbose2("socket bind to port '%d'", _httpd_port);

  /* init built-in services */
  /*
    httpd_register("/httpd/list", service_list);
  */

  /* create socket */
  hsocket_init(&_httpd_socket);
  status = hsocket_bind(&_httpd_socket, _httpd_port);

  return status;
}

/* ----------------------------------------------------- 
 FUNCTION: httpd_register
----------------------------------------------------- */

int httpd_register(const char* ctx,  httpd_service func)
{
  hservice_t* service;
  log_verbose3("register service:t(%p):%s", service, SAVE_STR(ctx));

  service = (hservice_t*)malloc(sizeof(hservice_t));
  service->next = NULL;
  service->func = func;
  strcpy(service->ctx, ctx);
  
  if (_httpd_services_head == NULL) {
    _httpd_services_head = _httpd_services_tail = service;
  } else {
    _httpd_services_tail->next = service;
    _httpd_services_tail = service;
  }

  return 1;
}


/* ----------------------------------------------------- 
 FUNCTION: httpd_services
----------------------------------------------------- */
hservice_t *httpd_services()
{
  return _httpd_services_head;
}


/* ----------------------------------------------------- 
 FUNCTION: httpd_find_service
----------------------------------------------------- */
static hservice_t *httpd_find_service(const char* ctx) 
{
  hservice_t *cur = _httpd_services_head;

  while (cur != NULL) {
    if (!strcmp(cur->ctx, ctx)) {
      return cur;
    }
    cur = cur->next;
  }

  return NULL;
}


/* ----------------------------------------------------- 
 FUNCTION: httpd_response_set_content_type
----------------------------------------------------- */
void httpd_response_set_content_type(httpd_conn_t *res, 
				     const char* content_type)
{
  strncpy(res->content_type, content_type, 25);
}


/* ----------------------------------------------------- 
 FUNCTION: httpd_response_send_header
----------------------------------------------------- */
int httpd_send_header(httpd_conn_t *res, 
		      int code, const char* text, 
		      hpair_t *pair)
{
  struct tm stm;
  time_t nw;
  char buffer[255];
  char header[1024];
  hpair_t *cur;
  int status;

  /* set status code */
  sprintf(header, "HTTP/1.1 %d %s\r\n", code, text);

  /* set date */
  nw = time(NULL);
  localtime_r(&nw, &stm);
  strftime(buffer, 255, "Date: %a, %d %b %y %T GMT", &stm);
  strcat(header, buffer);
  strcat(header, "\r\n");
  
  /* set content-type */
  /*
  if (res->content_type[0] == '\0') {
    strcat(header, "Content-Type: text/html\r\n");
  } else {
    sprintf(buffer, "Content-Type: %s\r\n", res->content_type);
    strcat(header, buffer);
  }
  */

  /* set server name */
  strcat(header, "Server: Nano HTTPD library\r\n");

  /* set connection status */
  strcat(header, "Connection: close\r\n");

  /* add pairs */
  cur = pair;
  while (cur != NULL) {
    sprintf(buffer, "%s: %s\r\n", cur->key, cur->value);
    strcat(header, buffer);
    cur = cur->next;
  }

  /* set end of header */
  strcat(header, "\r\n");

  /* send header */
  status = hsocket_nsend(res->sock, header, strlen(header));
  
  return status;
}


int httpd_send_internal_error(httpd_conn_t *conn, const char* errmsg)
{
  const char *template =
    "<html><body><h3>Error!</h3><hr> Message: '%s' </body></html>\r\n";

  char buffer[4064];
  sprintf(buffer, template, errmsg);
  httpd_send_header(conn, 500, "INTERNAL", NULL);
  return send(conn->sock, buffer, strlen(buffer), 0);
}

/* ----------------------------------------------------- 
 FUNCTION: httpd_request_print
----------------------------------------------------- */
static void httpd_request_print(hrequest_t *req) 
{
  hpair_t *pair;

  log_verbose1("++++++ Request +++++++++");
  log_verbose2(" Method : '%s'", req->method);
  log_verbose2(" Path   : '%s'", req->path);
  log_verbose2(" Spec   : '%s'", req->spec);
  log_verbose1(" Parsed query string :");

  pair = req->query;
  while (pair != NULL) {
    log_verbose3(" %s = '%s'", pair->key, pair->value);
    pair = pair->next;
  }
  log_verbose1("++++++++++++++++++++++++");
  
}

/* ----------------------------------------------------- 
 FUNCTION: httpd_session_main
----------------------------------------------------- */
static void* httpd_session_main(void *data)
{
  conndata_t *conn = (conndata_t*)data;
  const char *msg = "SESSION 1.0\n";
  int len = strlen(msg);
  char ch[2];
  char buffer[256]; /* temp buffer for recv() */
  char header[4064]; /* received header */
  int total; /* result from recv() */
  int hindex; /* searching end of header */
  int headerreached =0; /* whether reach header "\n\n" */
  hrequest_t* req = NULL; /* only for test */
  httpd_conn_t *rconn;
  hservice_t* service = NULL;
  char *content_length_str;
  long content_length = 0;

  header[0] = '\0';
  len = 0;


  log_verbose1("starting httpd_session_main()");

  while (len < 4064) {
    /*printf("receiving ...\n");*/
    total = recv(conn->sock, ch, 1, 0);
    if (total==0) break;
    header[len] = ch[0];
    len++;
    if (len > 3) {
      if (!strncmp(&header[len-4], "\r\n\r\n", 4)) {
	header[len] = '\0';
	break;
      }
    }
  }

  /*  log_verbose2("=== HEADER ===\n%s\n============\n", header);*/
  /* call the service */
  req = hrequest_new_from_buffer(header);
  httpd_request_print(req);


  rconn = (httpd_conn_t*)malloc(sizeof(httpd_conn_t));
  rconn->sock = conn->sock;
  rconn->content_type[0] = '\0';

  service = httpd_find_service(req->path);
  if (service != NULL) {
    log_verbose2("service '%s' found", req->path);
    if (service->func != NULL) {
      service->func(rconn, req);
    } else {
      sprintf(buffer, 
	      "service '%s' not registered properly (func == NULL)", 
	      req->path);
      log_verbose1(buffer);
      httpd_send_internal_error(rconn, buffer);
    }
  } else {
    sprintf(buffer, "service '%s' not found", req->path);
      log_verbose1(buffer);
      httpd_send_internal_error(rconn, buffer);
  }

  close(conn->sock);

  /*  httpd_response_free(res);*/
  hrequest_free(req);
  
  pthread_exit(NULL);
} 


/* ----------------------------------------------------- 
 FUNCTION: httpd_run
----------------------------------------------------- */

int httpd_run()
{
  conndata_t *conn;
  pthread_t tid;
  hsocket_t sockfd;
  int err;
  fd_set fds;
  /*struct timeval timeout;*/
  
  log_verbose1("starting run routine");
  /*
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  */
  /* listen to port */
  err = hsocket_listen(_httpd_socket,15);
  if (err  != HSOCKET_OK) {
    log_error2("httpd_run(): '%d'", err);
    return err;
  }


  log_verbose2("listening to port '%d'", _httpd_port);
  

  /*  fcntl(_httpd_socket, F_SETFL, O_NONBLOCK);*/

  while (1) {
    
    /*FD_ZERO(&fds);
    FD_SET(_httpd_socket, &fds);

    select(1, &fds, NULL, NULL, &timeout);

    while ( (FD_ISSET(_httpd_socket, &fds)))
      select(1, &fds, NULL, NULL, &timeout);
    */   
    
  
    if (hsocket_accept(_httpd_socket, &sockfd) != HSOCKET_OK) {
      continue;
    }


     conn = (conndata_t*)malloc(sizeof(conndata_t));
     conn->sock = sockfd;

     err = pthread_create(&tid, NULL, httpd_session_main, conn); 
     if (err) {
       log_error2("Error creating thread: ('%d')", err);
     }
  }

  return 0;
}




char *httpd_get_postdata(httpd_conn_t *conn, hrequest_t *req, long *received, long max)
{
  char *content_length_str;
  long content_length = 0;
  long total = 0;
  char *postdata = NULL;

  if (!strcmp(req->method, "POST")) {

    content_length_str = 
      hpairnode_get_ignore_case(req->header, HEADER_CONTENT_LENGTH);
    
    if (content_length_str != NULL)
      content_length = atol(content_length_str);

  } else {
    log_warn1("Not a POST method");
    return NULL;
  }      

  if (content_length > max && max != -1) 
    return NULL;

  if (content_length == 0) {
    *received = 0;
    postdata = (char*)malloc(1);
    postdata[0] = '\0';
    return postdata;
  }

  postdata = (char*)malloc(content_length+1);
  if (postdata == NULL) {
    log_error1("Not enough memory");
    return NULL;
  }


  if (hsocket_read(conn->sock, postdata, 
		   (int)content_length, 1) == HSOCKET_OK) {
    *received = content_length;
    postdata[content_length] = '\0';
    return postdata;
  }

  free (postdata);
  return NULL;
  
}