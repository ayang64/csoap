/******************************************************************
 *  $Id: nanohttp-client.c,v 1.3 2003/12/16 14:12:57 snowdrop Exp $
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
#include <nanohttp/nanohttp-client.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>

#include <stdio.h>
#include <stdlib.h>


/*--------------------------------------------------
  STRUCTURES
----------------------------------------------------*/
typedef struct httpc_cb_userdata
{
  httpc_conn_t *conn;
  void *userdata;
  httpc_response_callback callback;
  int counter;
}httpc_cb_userdata_t;


/*--------------------------------------------------
  FUNCTION: httpc_new
----------------------------------------------------*/
httpc_conn_t* httpc_new()
{
  httpc_conn_t* res = (httpc_conn_t*)malloc(sizeof(httpc_conn_t));

  hsocket_init(&res->sock);
  res->header = NULL;
  res->url = NULL;

  return res;
}


/*--------------------------------------------------
  FUNCTION: httpc_free
----------------------------------------------------*/
void httpc_free(httpc_conn_t* conn)
{
  const char *FUNC = "httpc_free";
  hpair_t *tmp;

  if (conn != NULL) {
    hsocket_free(conn->sock);
    

    
    while (conn->header != NULL) {    
      tmp = conn->header;
      conn->header = conn->header->next;
      hpairnode_free(tmp);
    }

    free(conn);
  }
}


/*--------------------------------------------------
  FUNCTION: httpc_add_header
----------------------------------------------------*/
void httpc_add_header(httpc_conn_t *conn, const char* key, const char* value)
{
  const char *FUNC = "httpc_add_header";

  if (conn == NULL) {
    log_warn(FUNC, "Connection object is NULL");
    return;
  }

  conn->header = hpairnode_new(key, value, conn->header);
}


/*--------------------------------------------------
  FUNCTION: httpc_set_header
----------------------------------------------------*/
int httpc_set_header(httpc_conn_t *conn, const char* key, const char* value)
{
  const char *FUNC = "httpc_set_header";
  hpair_t *p;

  if (conn == NULL) {
    log_warn(FUNC, "Connection object is NULL");
    return 0;
  }

  p = conn->header;
  while (p != NULL) {
    if (p->key != NULL) {
      if (!strcmp(p->key, key)) {
	free(p->value);
	p->value = (char*)malloc(strlen(value)+1);
	strcpy(p->value, value);
	return 1;
      }
    }
    p = p->next;
  }

  conn->header = hpairnode_new(key, value, conn->header);
  return 0;
}



/*--------------------------------------------------
  FUNCTION: httpc_build_header
----------------------------------------------------*/
static 
void httpc_build_header(httpc_conn_t *conn)
{
  char buffer[255];
  time_t nw;
  struct tm stm;
  
  /* Set date */
  nw = time(NULL);
  localtime_r(&nw, &stm);
  strftime(buffer, 255, "%a, %d %b %y %T GMT", &stm);
  httpc_set_header(conn, HEADER_DATE, buffer); 

}

/*--------------------------------------------------
  FUNCTION: httpc_get
----------------------------------------------------*/
hresponse_t *httpc_get(httpc_conn_t *conn, const char* urlstr)
{
  const char *FUNC = "httpc_get";

  hurl_t *url;
  char buffer[255];
  int status;
  char *response;
  int rsize;
  hresponse_t *res;
  char *rest;
  int restsize;

  if (conn == NULL) {
    log_error(FUNC, "Connection object is NULL");
    return NULL;
  }

  /* set response to 0 to allocate 
     it in hsocket_recv */
  response = 0;

  /* Build request header  */
  httpc_build_header(conn);

  /* Create url */
  url = hurl_new(urlstr);
  if (url == NULL) {
    log_error(FUNC, "Can not parse URL '%s'", SAVE_STR(urlstr));
    return NULL;
  }

  /* Set hostname  */
  httpc_set_header(conn, HEADER_HOST, url->host);

  /* Open connection */
  status = hsocket_open(&conn->sock, url->host, url->port);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not open connection to '%s' (status:%d)", 
	      SAVE_STR(url->host), status);
    return NULL;
  }

  /* Send GET  */
  sprintf(buffer, "GET %s HTTP/1.1\r\n", 
	  (url->context)?url->context:("/"));
  status = hsocket_send(conn->sock, buffer);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not send GET (status:%d)", status);
    hsocket_close(conn->sock);
    return NULL;
  }

  /* Send Header */
  status = httpc_send_header(conn);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not send header (status:%d)", status);
    hsocket_close(conn->sock);
    return NULL;
  }
  
  /* Receive Response */
  status = hsocket_recv(conn->sock, &response, &rsize);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not receive response (status:%d)", status);
    return NULL;
  } 
   

  /*status = hsocket_recv_limit(conn->sock, &response, 
			      "\r\n\r\n", &rest, &rsize, &restsize);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not receive response (status:%d)", status);
    return NULL;
  }
  */
  res = hresponse_new(response);

  if (res == NULL) {
    free(response);
    return NULL;
  }

  /*res->body = rest;*/
  return res;
}


/*--------------------------------------------------
  FUNCTION: httpc_send_header
----------------------------------------------------*/
int httpc_send_header(httpc_conn_t *conn)
{
  hpair_t *p;
  int status;
  char buffer[1024];

  p = conn->header;
  while (p != NULL) {
    if (p->key && p->value) {
      sprintf(buffer, "%s: %s\r\n", p->key, p->value);
      status = hsocket_send(conn->sock, buffer);
      if (status != HSOCKET_OK)
	return status;
    }
    p = p->next;
  }

  status = hsocket_send(conn->sock, "\r\n");
  return status;
}

/*--------------------------------------------------
  FUNCTION: httpc_post
----------------------------------------------------*/
hresponse_t *httpc_post(httpc_conn_t *conn, const char *urlstr, const char *content)
{
  const char *FUNC = "httpc_post";
  int content_length;
  hurl_t *url;
  char buffer[255];
  int status;
  char *response;
  hresponse_t *res;
  int rsize;

  if (conn == NULL) {
    log_error(FUNC, "Connection object is NULL");
    return NULL;
  }

  if (content == NULL) {
    log_error(FUNC, "Content is NULL");
    return NULL;
  }

  /* set response to 0 to allocate 
     it in hsocket_recv */
  response = 0;

  /* Build request header  */
  httpc_build_header(conn);

  /* Create url */
  url = hurl_new(urlstr);
  if (url == NULL) {
    log_error(FUNC, "Can not parse URL '%s'", SAVE_STR(urlstr));
    return NULL;
  }

  /* Set content length */
  content_length = strlen(content);
  sprintf(buffer, "%d", content_length);
  httpc_set_header(conn, HEADER_CONTENT_LENGTH, buffer);

  /* Set hostname  */
  httpc_set_header(conn, HEADER_HOST, url->host);

  /* Open connection */
  status = hsocket_open(&conn->sock, url->host, url->port);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not open connection to '%s' (status:%d)", 
	      SAVE_STR(url->host), status);
    return NULL;
  }

  /* Send POST  */
  sprintf(buffer, "POST %s HTTP/1.1\r\n", 
	  (url->context)?url->context:("/"));
  status = hsocket_send(conn->sock, buffer);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not send POST (status:%d)", status);
    hsocket_close(conn->sock);
    return NULL;
  }

  /* Send Header */
  status = httpc_send_header(conn);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not send header (status:%d)", status);
    hsocket_close(conn->sock);
    return NULL;
  }
  
  /* Send Content */
  status = hsocket_send(conn->sock, content);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not send content (status:%d)", status);
    return NULL;
  }

  /* Receive Response */
  status = hsocket_recv(conn->sock, &response, &rsize);
  if (status != HSOCKET_OK) {
    log_error(FUNC, "Can not receive response (status:%d)", status);
    return NULL;
  }

  res = hresponse_new(response);
  if (res == NULL) {
    free(response);
    return NULL;
  }

  return res;
}


/*--------------------------------------------------
  FUNCTION: httpc_recv_cb_callback
----------------------------------------------------*/
int httpc_recv_cb_callback(hsocket_t sock, char *buffer, 
			   int size, void *userdata)
{
  httpc_cb_userdata_t *cbdata = (httpc_cb_userdata_t*)userdata;

  cbdata->callback(cbdata->counter++, cbdata->conn, 
		   cbdata->userdata, size, buffer);
  return 1;
}

/*--------------------------------------------------
  FUNCTION: httpc_get_cb
----------------------------------------------------*/
int httpc_get_cb(httpc_conn_t *conn, const char *urlstr, 
		 httpc_response_start_callback start_cb,
		 httpc_response_callback cb, void *userdata)
{
  hurl_t *url;
  char buffer[4096];
  int status;
  char *response;
  int rsize;
  hresponse_t *res;
  char *rest;
  int restsize;
  httpc_cb_userdata_t cbdata;
  int i;

  /* content-length */
  char *content_length_str;
  long content_length;
  long remain_length;
  int counter;
  int recvSize;
  char readBuf[HSOCKET_MAX_BUFSIZE];

  /* chunked encoding */
  char *transfer_encoding;
  char *chunk_buffer;
  int chunk_size, cs;
  hbufsocket_t bufsock;
  char tmpch;
  char chunk_size_str[25];
  int chunk_size_cur;
  char chunk_ch[2];

  if (conn == NULL) {
    log_error1("Connection object is NULL");
    return 1;
  }

  /* set response to 0 to allocate 
     it in hsocket_recv */
  response = 0;

  /* Build request header  */
  httpc_build_header(conn);

  /* Create url */
  url = hurl_new(urlstr);
  if (url == NULL) {
    log_error2("Can not parse URL '%s'", SAVE_STR(urlstr));
    return 2;
  }

  /* Set hostname  */
  httpc_set_header(conn, HEADER_HOST, url->host);

  /* Open connection */
  status = hsocket_open(&conn->sock, url->host, url->port);
  if (status != HSOCKET_OK) {
    log_error3("Can not open connection to '%s' (status:%d)", 
	      SAVE_STR(url->host), status);
    return 3;
  }

  /* Send GET  */
  sprintf(buffer, "GET %s HTTP/1.1\r\n", 
	  (url->context)?url->context:("/"));
  status = hsocket_send(conn->sock, buffer);
  if (status != HSOCKET_OK) {
    log_error2("Can not send GET (status:%d)", status);
    hsocket_close(conn->sock);
    return 4;
  }

  /* Send Header */
  status = httpc_send_header(conn);
  if (status != HSOCKET_OK) {
    log_error2("Can not send header (status:%d)", status);
    hsocket_close(conn->sock);
    return 5;
  }
  
  /* Receive Response incl. header */
  rsize = restsize = 0;
  response = rest = NULL;

  status = hsocket_read(conn->sock, buffer, HSOCKET_MAX_BUFSIZE*1, 0);
  if (status <= 0) {
    log_error2("Can not receive response (status:%d)", status); 
    return 6;
  } 

  for (i=0;i<status-3;i++) {

    if (buffer[i] == '\n') {
      if (buffer[i+1] == '\n') {
	rsize = i;
	response = buffer;
	response[rsize] = '\0';

	restsize = status - rsize - 2;
	rest = &buffer[i+2];
	rest[restsize] = '\0';
      } else if (buffer[i+1] == '\r' &&  buffer[i+2] == '\n') {
      
	rsize = i;
	response = buffer;
	response[rsize] = '\0';

	restsize = status - rsize - 3;
	rest = &buffer[i+3];
	rest[restsize] = '\0'; /* REST WILL BE FREED IN BUFFEREDSOCKET !!!! */
      }
    }
  }

  if (response == NULL) {
    log_error1("Header too long!");
    return 13;
  }
  
  res = hresponse_new(response);
  if (res == NULL) {
    log_error2("Can't create response (url:'%s')", urlstr);
    return 7;
  }

  /* Invoke callback */
  start_cb(conn, userdata, res->header, res->spec, res->errcode, res->desc);


  /* ================================================= */
  /*   Retreive with content-length                    */ 
  /* ================================================= */

  /* Check if server communicates with content-length */
  content_length_str = 
    hpairnode_get(res->header, HEADER_CONTENT_LENGTH);

  if (content_length_str != NULL) {

    log_debug1("Server communicates with content-length!");
    
    /* Invoke callback for rest */
    if (restsize > 0)
      cb(0, conn, userdata, restsize, rest);

    /* content length */
    content_length = atol(content_length_str);

    counter = 1;
    remain_length = content_length - restsize;
    while (remain_length > 0) {
      if (remain_length >= HSOCKET_MAX_BUFSIZE) {
	recvSize = HSOCKET_MAX_BUFSIZE;
      } else {
	recvSize = remain_length;
      }

      if (hsocket_read(conn->sock, readBuf, recvSize,1)) {
	log_error1("Can not read from socket!");
	return 9;
      } else {
	cb(counter++, conn, userdata, recvSize, readBuf);
      }

      remain_length -= HSOCKET_MAX_BUFSIZE;

    } /* while */

    /* rest and response are no longer required */

    return 0;

  } /* if content length */


  /* ================================================= */
  /*   Retreive with chunked encoding                  */ 
  /* ================================================= */

  /* Check if server communicates with chunked encoding */
  transfer_encoding = 
    hpairnode_get(res->header, HEADER_TRANSFER_ENCODING);
  if (transfer_encoding != NULL && 
      !strcmp(transfer_encoding, "chunked")) { 

    log_debug1("Server communicates with chunked encoding !");

    /* initialize buffered socket */
    bufsock.sock = conn->sock;
    bufsock.cur = 0;
    bufsock.buffer = rest;
    bufsock.bufsize = restsize;
    
    while (chunk_size > 0) {

      /* read chunk size */
      chunk_size_cur = 0;
      while (1) {

	
	if (hbufsocket_read(&bufsock, &chunk_size_str[chunk_size_cur], 1)) {
	  log_error1("Can not read from socket");
	  return 9;
	}
	
	if (chunk_size_str[chunk_size_cur] == '\n') {
	    chunk_size_str[chunk_size_cur] = '\0';
	    break;
	}

	if (chunk_size_str[chunk_size_cur] != '\r'
	    && chunk_size_str[chunk_size_cur] != ';') {
	  chunk_size_cur++;
	}

      } /* while (1) */

      chunk_size = strtol(chunk_size_str,(char**)NULL, 16); /* hex to dec */
      log_debug3("chunk_size: '%s' as dec: '%d'", 
		 chunk_size_str, chunk_size);

      if (chunk_size <= 0) break;

      chunk_buffer = (char*)malloc(chunk_size);
      if (hbufsocket_read(&bufsock, chunk_buffer, chunk_size)) {
	log_error1("Can not read from socket");
	return 9;
      }
      cb(counter++, conn, userdata, chunk_size, chunk_buffer);
      free(chunk_buffer);

    } /* while (chunk_size > 0) */
    
    /* rest and response are no longer required */
    
    return 0;

  } /* if transfer_encodig */
  

  log_error1("Unknown server response retreive type!");

  return 1;
}
	
	 
/*--------------------------------------------------
  FUNCTION: httpc_post_cb
----------------------------------------------------*/
int httpc_post_cb(httpc_conn_t *conn, const char *url, 
		  httpc_response_start_callback start_cb,
		  httpc_response_callback cb, void *userdata)
{
  return 1;
}
		 





