/******************************************************************
 *  $Id: nanohttp-client.c,v 1.12 2004/06/04 08:55:07 snowdrop Exp $
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
  FUNCTION: httpc_new
  DESC: Creates a new http client connection object
  You need to create at least 1 http client connection
  to communicate via http.
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
  DESC: Free the given http client object.
----------------------------------------------------*/
void httpc_free(httpc_conn_t* conn)
{
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
  FUNCTION: httpc_set_header
  DESC: Adds a new (key, value) pair to the header
  or modifies the old pair if this function will 
  finds another pair with the same 'key' value.
----------------------------------------------------*/
int httpc_set_header(httpc_conn_t *conn, const char* key, const char* value)
{
  hpair_t *p;

  if (conn == NULL) {
    log_warn1("Connection object is NULL");
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
  FUNCTION: httpc_header_add_date
  DESC: Adds the current date to the header. 
----------------------------------------------------*/
static 
void httpc_header_add_date(httpc_conn_t *conn)
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
  FUNCTION: httpc_send_header
  DESC: Sends the current header information stored
  in conn through conn->sock.
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

static
hresponse_t *httpc_receive_header(hsocket_t sock)
{
  hresponse_t *res;
  int done;
  int i;
  int status;
  char buffer[HSOCKET_MAX_BUFSIZE];
  char *response;
  char *rest;
  int rsize;
  int restsize;

  /* Receive Response incl. header */
  rsize = restsize = 0;
  response = rest = NULL;
  done = 0;

  while (!done) {
    
    status = hsocket_read(sock, buffer, HSOCKET_MAX_BUFSIZE, 0);
  
    if (status <= 0) {
      log_error2("Can not receive response (status:%d)", status); 
      return NULL;
    } 


    for (i=0;i<status-2;i++) {

      /*  log_debug5("%d -> '%c' (%d, %d)", buffer[i], buffer[i], 
       	 buffer[i+1], buffer[i+2]);
      */
      if (buffer[i] == '\n') {
	if (buffer[i+1] == '\n') {
	  
	  response = (char*)realloc(response, rsize+i+1);
	  strncpy(&response[rsize], buffer, i);
	  response[rsize+i] = '\0';
	  rsize += i;
	
	  restsize = status - i - 2;
	  rest = (char*)malloc(restsize+1);
	  strncpy(rest, &buffer[i+2], restsize);
	  rest[restsize] = '\0';
	  done = 1;
	  break;

	} else if (buffer[i+1] == '\r' &&  buffer[i+2] == '\n') {
	
	  response = (char*)realloc(response, rsize+i+1);
	  strncpy(&response[rsize], buffer, i);
	  response[rsize+i] = '\0';
	  rsize += i;
	  
	  restsize = status - i - 3;
	  rest = (char*)malloc(restsize+1);
	  strncpy(rest, &buffer[i+3], restsize);
	  rest[restsize] = '\0';
	  done = 1;
	  break;
	}
      }
    }

    if (!done)
      rsize += status;
  }


  if (response == NULL) {
    log_error1("Header too long!");
    return NULL;
  }
  
  res = hresponse_new_from_buffer(response);
  if (res == NULL) {
    log_error1("Can't create response");
    return NULL;
  }

  res->bodysize = restsize;
  res->body = rest;

  if (res->errcode == 100) { /* continue */
    hresponse_free(res);
    res = httpc_receive_header(sock);
  }
  return res;
}


static
int httpc_receive_with_connection_closed(httpc_conn_t *conn,
					 hresponse_t *res,
					 httpc_response_callback cb, 
					 void *userdata)
{
  /* connection closed */
  char *connection_status;
  int status;
  char buffer[HSOCKET_MAX_BUFSIZE];
  int counter;

  counter = 0;

  /* ================================================= */
  /*   Retreive with only "Connection: close"          */ 
  /* ================================================= */
  connection_status = 
    hpairnode_get(res->header, HEADER_CONNECTION);

  if (connection_status != NULL && 
      !strcmp(connection_status, "close")) { 

    log_debug1("Server communicates with 'Connection: close' !");

    /* Invoke callback for rest */
    if (res->bodysize > 0)
      cb(0, conn, userdata, res->bodysize, res->body);


    while (1) {
      
      status = hsocket_read(conn->sock, buffer, HSOCKET_MAX_BUFSIZE, 0);

      if (status == 0) { /* close connection */
        close(conn->sock);
	return 0; 
      }
    
      if (status < 0) { /* error */
	log_error2("Can nor read from socket (status: %d)", status);
	return 11;
      }

      /* Invoke callback */ 
      cb(counter++, conn, userdata, status, buffer);
    }

    return 0;
  }

  return -1;
}

static
int httpc_receive_with_chunked_encoding(httpc_conn_t *conn,
					hresponse_t *res,
					httpc_response_callback cb, 
					void *userdata)
{
  /* chunked encoding */
  char *transfer_encoding;
  char *chunk_buffer;
  int chunk_size;
  hbufsocket_t bufsock;
  char chunk_size_str[25];
  int chunk_size_cur;
  int counter;
  char buffer[2];
  
  counter = 0;
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
    bufsock.buffer = res->body;
    bufsock.bufsize = res->bodysize;
    
    chunk_size = 1;
    while (chunk_size > 0) {

      /* read chunk size */
      chunk_size_cur = 0;
      while (1) {

	
	if (hbufsocket_read(&bufsock, &chunk_size_str[chunk_size_cur], 1)) {
	  log_error1("Can not read from socket");
	  return 9;
	}
	
	log_debug2("chunk_size_str[chunk_size_cur] = '%c'", 
		   chunk_size_str[chunk_size_cur]);

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

      chunk_buffer = (char*)malloc(chunk_size+1);
      if (hbufsocket_read(&bufsock, chunk_buffer, chunk_size)) {
	log_error1("Can not read from socket");
	return 9;
      }
      cb(counter++, conn, userdata, chunk_size, chunk_buffer);
      free(chunk_buffer);

      /* skip new line */
      buffer[0] = 0; /* reset buffer[0] */
      while (buffer[0] != '\n') {
	hbufsocket_read(&bufsock, &buffer[0], 1);
      }

    } /* while (chunk_size > 0) */
    
    /* rest and response are no longer required */
    
    return 0;

  } /* if transfer_encodig */
  
  return -1;
}

/*
  returns -1 if server does not communicate with 
  content-length;
 */
static 
int httpc_receive_with_content_length(httpc_conn_t *conn,
				      hresponse_t *res,
				      httpc_response_callback cb, 
				      void *userdata)
{
  int counter;
  int content_length;
  int remain_length;
  int recvSize;
  char *content_length_str;
  char buffer[HSOCKET_MAX_BUFSIZE];

  /* ================================================= */
  /*   Retreive with content-length                    */ 
  /* ================================================= */

  /* Check if server communicates with content-length */
  content_length_str = 
    hpairnode_get_ignore_case(res->header, HEADER_CONTENT_LENGTH);

  if (content_length_str != NULL) {

    log_debug1("Server communicates with content-length!");
    
    /* Invoke callback for rest */
    if (res->bodysize > 0)
      cb(0, conn, userdata, res->bodysize, res->body);

    /* content length */
    content_length = atol(content_length_str);

    counter = 1;
    remain_length = content_length - res->bodysize;
    while (remain_length > 0) {
      if (remain_length >= HSOCKET_MAX_BUFSIZE) {
	recvSize = HSOCKET_MAX_BUFSIZE;
      } else {
	recvSize = remain_length;
      }

      if (hsocket_read(conn->sock, buffer, recvSize,1)) {
	log_error1("Can not read from socket!");
	return 9;
      } else {
	cb(counter++, conn, userdata, recvSize, buffer);
      }

      remain_length -= HSOCKET_MAX_BUFSIZE;

    } /* while */

    /* rest and response are no longer required */

    return 0;

  } /* if content length */

  return -1;
}

static
int httpc_receive_response(httpc_conn_t *conn, 
			 httpc_response_start_callback start_cb,
			 httpc_response_callback cb, void *userdata)
{
  hresponse_t *res;
  int status;

  /* receive header */
  log_verbose1("receiving header");
  res = httpc_receive_header(conn->sock);
  if (res == NULL) return 1;
  log_verbose1("header ok");

  /* Invoke callback */
  start_cb(conn, userdata, res->header, res->spec, 
	   res->errcode, res->desc);

  /* try to receive with content length */
  status = httpc_receive_with_content_length(conn, res, 
					     cb, userdata);
  if (status != -1) {
    hresponse_free(res);
    return status;
  }

  status = httpc_receive_with_chunked_encoding(conn, res,
					       cb, userdata);

  if (status != -1) {
    hresponse_free(res);
    return status;
  }

  status = httpc_receive_with_connection_closed(conn, res,
						cb, userdata);
  if (status != -1) {
    hresponse_free(res);
    return status;
  }

  log_error1("Unknown server response retreive type!");

}

/*--------------------------------------------------
  FUNCTION: httpc_talk_to_server
  DESC: This function is the heart of the httpc 
  module. It will send the request and process the
  response. 

  Here the parameters:

  method:
   the request method. This can be HTTP_REQUEST_POST and 
   HTTP_REQUEST_GET.

  conn: 
   the connection object (created with httpc_new())
   
  urlstr:
   the complete url in string format.
   http://<host>:<port>/<context>
   where <port> is not mendatory.

  start_cb:
   a callback function, which will be called when
   the response header is completely arrives.

  cb:
   a callback function, which will be called everytime
   when data arrives.

  content_size:
   size of content to send. 
   (only if method is HTTP_REQUEST_POST)

  content:
   the content data to send. 
   (only if method is HTTP_REQUEST_POST)

  userdata:
   a user define data, which will be passed to the
   start_cb and cb callbacks as a parameter. This
   can also be NULL.

   
  If success, this function will return 0. 
  >0 otherwise.
----------------------------------------------------*/
static
int httpc_talk_to_server(hreq_method method, httpc_conn_t *conn, 
			 const char *urlstr)
{
  hurl_t *url;
  char buffer[4096];
  int status;


  if (conn == NULL) {
    log_error1("Connection object is NULL");
    return 1;
  }

  /* Build request header */
  httpc_header_add_date(conn);

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

  /* check method */
  if (method == HTTP_REQUEST_GET) {

    /* Set GET Header  */
    sprintf(buffer, "GET %s HTTP/1.1\r\n", 
	    (url->context)?url->context:("/"));

  } else if (method == HTTP_REQUEST_POST) {

    /* Set POST Header  */
    sprintf(buffer, "POST %s HTTP/1.1\r\n", 
	    (url->context)?url->context:("/"));
  } else {
    
    log_error1("Unknown method type!");
    return 15;
  }

  status = hsocket_send(conn->sock, buffer);
  if (status != HSOCKET_OK) {
    log_error2("Can not send request (status:%d)", status);
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

  return status;
  
}
	
	 
/*--------------------------------------------------
  FUNCTION: httpc_get_cb
  DESC: Wraps the httpc_talk_to_server() function
  to communicate with GET method. 

  See the documentation of httpc_talk_to_server()
  for more information.
----------------------------------------------------*/
int httpc_get_cb(httpc_conn_t *conn, const char *urlstr, 
		 httpc_response_start_callback start_cb,
		 httpc_response_callback cb, void *userdata)
{
  int status;

  status = httpc_talk_to_server(HTTP_REQUEST_GET, conn, urlstr);

  if (status != HSOCKET_OK)
    return status;

  status = httpc_receive_response(conn, start_cb, cb, userdata);
  return status;
}



/*--------------------------------------------------
  FUNCTION: httpc_post_cb
  DESC: Wraps the httpc_talk_to_server() function
  to communicate with POST method. 

  See the documentation of httpc_talk_to_server()
  for more information.

  TODO: Add post content rutine
----------------------------------------------------*/
int httpc_post_cb(httpc_conn_t *conn, const char *urlstr, 
		  httpc_response_start_callback start_cb,
		  httpc_response_callback cb, int content_size, 
		  char *content,  void *userdata)
{
  int status;
  char buffer[255];

  sprintf(buffer, "%d", content_size);
  httpc_set_header(conn, HEADER_CONTENT_LENGTH, buffer);

  status = httpc_talk_to_server(HTTP_REQUEST_POST, conn, urlstr);
  if (status != HSOCKET_OK) 
    return status; 

  status = hsocket_nsend(conn->sock, content, content_size);
  if (status != HSOCKET_OK)
    return status;

  status = httpc_receive_response(conn, start_cb, cb, userdata); 
  return status; 
}
		 

/*======================================================
  The following  functions are used internally to wrap 
  the httpc_x_cb (x = get|post)  functions. 
  ======================================================*/
static
void httpc_custom_res_callback(int counter, httpc_conn_t* conn,
			       void *userdata, int size, char *buffer)
{
  hresponse_t *res;

  /* get response object */
  res = (hresponse_t*)userdata;

  /* allocate buffersize */

  res->bodysize += size;
  res->body = (unsigned char*)realloc(res->body, res->bodysize + 1);

  memcpy(&(res->body[res->bodysize-size]), buffer, size);
  res->body[res->bodysize]='\0';
}


static 
void httpc_custom_start_callback(httpc_conn_t *conn, void *userdata, 
				 hpair_t *header, const char *spec,
				 int errcode, const char *desc)
{
  hresponse_t *res;

  /* get response object */
  res = (hresponse_t*)userdata;

  /* set spec */
  if (spec != NULL) {
    strcpy(res->spec, spec);
  }

  /* set errcode */
  res->errcode = errcode;

  /* set desc */
  if (desc != NULL) {
    res->desc = (char*)malloc(strlen(desc)+1);
    strcpy(res->desc, desc);
  }

  /* set header */
  if (header == NULL) {
    log_warn1("header is NULL!");
  }

  res->header = hpairnode_copy_deep(header);
}
     

/*--------------------------------------------------
  FUNCTION: httpc_get
----------------------------------------------------*/
hresponse_t *httpc_get(httpc_conn_t *conn, const char *url)
{
  int status;
  hresponse_t *res;

  res = hresponse_new();
  status = httpc_get_cb(conn, url, httpc_custom_start_callback,
			httpc_custom_res_callback, res);

  if (status != 0) {
    hresponse_free(res);
    return NULL;
  }

  return res;
} 


/*--------------------------------------------------
  FUNCTION: httpc_post
----------------------------------------------------*/
hresponse_t *httpc_post(httpc_conn_t *conn, const char *url, 
			int content_size, const char *content)
{ 
  int status;
  hresponse_t *res;

  res = hresponse_new();
  status = httpc_post_cb(conn, url, httpc_custom_start_callback,
			 httpc_custom_res_callback, content_size,
			 content, res);

  if (status != 0) {
    hresponse_free(res);
    return NULL;
  }

  return res;
} 



/*
  POST Module
 */



/*--------------------------------------------------
  FUNCTION: httpc_post_open
----------------------------------------------------*/
int httpc_post_open(httpc_conn_t *conn, const char *urlstr)
{
  int status;

  httpc_set_header(conn, HEADER_TRANSFER_ENCODING, "chunked");

  status = httpc_talk_to_server(HTTP_REQUEST_POST, conn, urlstr);
  return status;
}


/*--------------------------------------------------
  FUNCTION: httpc_post_send
----------------------------------------------------*/
int httpc_post_send(httpc_conn_t *conn, 
		    const char* buffer, 
		    int bufsize)
{
  char hexsize[100]; /* chunk size in hex */
  int status;

  sprintf(hexsize, "%x\r\n", bufsize); 
  
  status = hsocket_send(conn->sock, hexsize);
  if (status != HSOCKET_OK) 
    return status;

  status = hsocket_nsend(conn->sock, buffer, bufsize);
  if (status != HSOCKET_OK) 
    return status;

  status = hsocket_send(conn->sock, "\r\n");
  
  return status;
}


/*--------------------------------------------------
  FUNCTION: httpc_post_finish
----------------------------------------------------*/
hresponse_t *httpc_post_finish(httpc_conn_t *conn)
{
  int status;
  hresponse_t *res;

  res = hresponse_new();
  status = httpc_post_finish_cb(conn, httpc_custom_start_callback,
			 httpc_custom_res_callback, res);

  if (status != 0) {
    hresponse_free(res);
    return NULL;
  }

  return res;  
}


/*--------------------------------------------------
  FUNCTION: httpc_post_finish_cb
----------------------------------------------------*/
int httpc_post_finish_cb(httpc_conn_t *conn, 
			 httpc_response_start_callback start_cb,
			 httpc_response_callback cb, 
			 void *userdata)
{
  int status;

  status = hsocket_send(conn->sock, "0\r\n\r\n");
  if (status != HSOCKET_OK) return status;
  
  status = httpc_receive_response(conn, start_cb, cb, userdata); 
  return status; 
}






















