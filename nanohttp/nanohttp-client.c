/******************************************************************
*  $Id: nanohttp-client.c,v 1.30 2005/12/19 14:18:26 snowdrop Exp $
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
#include <nanohttp/nanohttp-client.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef MEM_DEBUG
#include <utils/alloc.h>
#endif

#if 0
static 
int  httpc_send_data(httpc_conn_t *conn, const unsigned char* data, size_t size)
{
  return -1;
}
#endif
/*--------------------------------------------------
FUNCTION: httpc_init
DESC: Initialize http client connection
NOTE: This will be called from soap_client_init_args()
----------------------------------------------------*/
herror_t 
httpc_init(int argc, char *argv[])
{
	hoption_init_args(argc, argv);
	hsocket_module_init();
  return H_OK;
}


/*--------------------------------------------------
FUNCTION: httpc_destroy
DESC: Destroy the http client module
----------------------------------------------------*/
void httpc_destroy()
{
  hsocket_module_destroy();
}


/*--------------------------------------------------
FUNCTION: httpc_new
DESC: Creates a new http client connection object
You need to create at least 1 http client connection
to communicate via http.
----------------------------------------------------*/
httpc_conn_t   *
httpc_new()
{
  static int counter = 10000;
	httpc_conn_t   *res = (httpc_conn_t *) malloc(sizeof(httpc_conn_t));

	if(hsocket_init(&res->sock)!= H_OK){
        return NULL;
    }
	res->header = NULL;
	res->version = HTTP_1_1;
	res->out = NULL;
	res->_dime_package_nr = 0;
	res->_dime_sent_bytes = 0;
	res->id = counter++;
	res->block = 0;
	return res;
}


/*--------------------------------------------------
FUNCTION: httpc_free
DESC: Free the given http client object.
----------------------------------------------------*/
void 
httpc_free(httpc_conn_t * conn)
{
	hpair_t        *tmp;

	if (conn == NULL) 
	 return;


  while (conn->header != NULL) 
  {
    tmp = conn->header;
    conn->header = conn->header->next;
    hpairnode_free(tmp);
  }

	if (conn->out != NULL)
	{
	  http_output_stream_free(conn->out);
	  conn->out = NULL;
	}

	hsocket_free(conn->sock);
	free(conn);

}

/*--------------------------------------------------
 FUNCTION: httpc_close_free
 DESC: Close and free the given http client object.
 ----------------------------------------------------*/
void
httpc_close_free(httpc_conn_t * conn)
{
      if (conn == NULL)
              return;

       hsocket_close(conn->sock);
       httpc_free(conn);
}


/*--------------------------------------------------
FUNCTION: httpc_set_header
DESC: Adds a new (key, value) pair to the header
or modifies the old pair if this function will
finds another pair with the same 'key' value.
----------------------------------------------------*/
int 
httpc_set_header(httpc_conn_t * conn, const char *key, const char *value)
{
	hpair_t        *p;

	if (conn == NULL) {
		log_warn1("Connection object is NULL");
		return 0;
	}
	p = conn->header;
	while (p != NULL) {
		if (p->key != NULL) {
			if (!strcmp(p->key, key)) {
				free(p->value);
				p->value = (char *) malloc(strlen(value) + 1);
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
void _httpc_set_error(httpc_conn_t *conn, int errcode, 
  const char *format, ...)
{
	va_list         ap;

  conn->errcode = errcode;

	va_start(ap, format);
	vsprintf(conn->errmsg,  format, ap);
	va_end(ap);
}

/*--------------------------------------------------
FUNCTION: httpc_header_add_date
DESC: Adds the current date to the header.
----------------------------------------------------*/
static
void 
httpc_header_add_date(httpc_conn_t * conn)
{
	char            buffer[255];
	time_t          nw;
	struct tm       stm;

	/* Set date */
	nw = time(NULL);
	localtime_r(&nw, &stm);
	strftime(buffer, 255, "%a, %d %b %Y %T GMT", &stm);
	httpc_set_header(conn, HEADER_DATE, buffer);

}


/*--------------------------------------------------
FUNCTION: httpc_send_header
DESC: Sends the current header information stored
in conn through conn->sock.
----------------------------------------------------*/
herror_t 
httpc_send_header(httpc_conn_t * conn)
{
	hpair_t        *p;
	herror_t        status;
	char            buffer[1024];

	p = conn->header;
	while (p != NULL) {
		if (p->key && p->value) {
			sprintf(buffer, "%s: %s\r\n", p->key, p->value);
			status = hsocket_send(conn->sock, buffer);
			if (status != H_OK)
				return status;
		}
		p = p->next;
	}

	status = hsocket_send(conn->sock, "\r\n");
	return status;
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
herror_t 
httpc_talk_to_server(hreq_method_t method, httpc_conn_t * conn,
		     const char *urlstr)
{

	hurl_t          url;
	char            buffer[4096];
	herror_t             status;

	if (conn == NULL) {
		return herror_new(
				"httpc_talk_to_server", 
				GENERAL_INVALID_PARAM, 
				"httpc_conn_t param is NULL");
	}
	/* Build request header */
	httpc_header_add_date(conn);

	/* Create url */
	status = hurl_parse(&url, urlstr);
	if (status != H_OK) {
		log_error2("Can not parse URL '%s'", SAVE_STR(urlstr));
		return status;
	}
/* TODO (#1#): Check for HTTP protocol in URL */

	/* Set hostname  */
	httpc_set_header(conn, HEADER_HOST, url.host);

	/* Open connection */
	status = hsocket_open(&conn->sock, url.host, url.port);
	if (status != H_OK) {
		return status;
	}
    /* TODO XXX XXX this is probably not right -- matt */
    if(!&conn->sock.ssl){
        status = hsocket_block(conn->sock, conn->block);
        if (status != H_OK) {
            log_error1("Cannot make socket non-blocking");
            return status;
        }
    }
	/* check method */
	if (method == HTTP_REQUEST_GET) {

		/* Set GET Header  */
		sprintf(buffer, "GET %s HTTP/%s\r\n",
			(url.context[0] != '\0') ? url.context : ("/"), 
			(conn->version == HTTP_1_0)?"1.0":"1.1");

	} else if (method == HTTP_REQUEST_POST) {

		/* Set POST Header  */
		sprintf(buffer, "POST %s HTTP/%s\r\n",
			(url.context[0] != '\0') ? url.context : ("/"),
			(conn->version == HTTP_1_0)?"1.0":"1.1");
	} else {

		log_error1("Unknown method type!");
		return herror_new(
				"httpc_talk_to_server", 
				GENERAL_INVALID_PARAM, 
				"hreq_method_t must be  HTTP_REQUEST_GET or HTTP_REQUEST_POST");
	}

    log_verbose1("Sending header...");
	status = hsocket_send(conn->sock, buffer);
	if (status != H_OK) {
		log_error2("Can not send request (status:%d)", status);
		hsocket_close(conn->sock);
		return status;
	}
	/* Send Header */
	status = httpc_send_header(conn);
	if (status != H_OK) {
		log_error2("Can not send header (status:%d)", status);
		hsocket_close(conn->sock);
		return status;
	}

	return H_OK;

}

/*--------------------------------------------------
FUNCTION: httpc_get
DESC:
----------------------------------------------------*/
herror_t
httpc_get(httpc_conn_t *conn, hresponse_t** out, const char *urlstr)
{
	herror_t             status;

	status = httpc_talk_to_server(HTTP_REQUEST_GET, conn, urlstr);

	if (status != H_OK)
	{
  	return status;
	}

	status = hresponse_new_from_socket(conn->sock, out);
	if (status != H_OK)
	{
  	return status;
	}
		

	return H_OK;
}


/*--------------------------------------------------
FUNCTION: httpc_post_begin
DESC: Returns H_OK if success
----------------------------------------------------*/
herror_t httpc_post_begin(httpc_conn_t *conn, const char *url)
{
  
	herror_t             status;

	status = httpc_talk_to_server(HTTP_REQUEST_POST, conn, url);
	if (status != H_OK)
		return status;

  conn->out = http_output_stream_new(conn->sock, conn->header);

  return H_OK;
}


/*--------------------------------------------------
FUNCTION: httpc_post_begin
DESC: End a "POST" method and receive the response.
  You MUST call httpc_post_end() before!
----------------------------------------------------*/
herror_t httpc_post_end(httpc_conn_t *conn, hresponse_t** out)
{
  herror_t status;

  status = http_output_stream_flush(conn->out);

  if (status != H_OK)
  {
    return status;
  }

	status = hresponse_new_from_socket(conn->sock, out);
	if (status != H_OK)
	{
    	return status;
	}

	return H_OK;
}



/* ---------------------------------------------------
  DIME support functions httpc_dime_* function set
-----------------------------------------------------*/
/*
int httpc_dime_begin(httpc_conn_t *conn, const char *url)
{
  int status;

	httpc_set_header(conn, HEADER_CONTENT_TYPE, "application/dime");

	status = httpc_talk_to_server(HTTP_REQUEST_POST, conn, url);
	if (status != H_OK)
	 return status;

  conn->out = http_output_stream_new(conn->sock, conn->header);
	
  return H_OK;
} 

static _print_binary_ascii(int n)
{
  int i,c=0;
  char ascii[36];

  for (i=0;i<32;i++) {
    ascii[34-i-c] = (n & (1<<i))?'1':'0';
    if ((i+1)%8 == 0) {
        c++;
        ascii[i+c] = ' ';
    }
  }

  ascii[35]='\0';

  log_verbose2("%s", ascii);
}

static 
void _get_binary_ascii8(unsigned char n, char* ascii)
{
  int i;
  for (i=0;i<8;i++)
    ascii[7-i] = (n & (1<<i))?'1':'0';

  ascii[8]='\0';
}

static
void _print_binary_ascii32(unsigned char b1, unsigned char b2,
                           unsigned char b3, unsigned char b4)
{
  char ascii[4][9];
  _get_binary_ascii8(b1, ascii[0]);
  _get_binary_ascii8(b2, ascii[1]);
  _get_binary_ascii8(b3, ascii[2]);
  _get_binary_ascii8(b4, ascii[3]);

  log_verbose5("%s %s %s %s", ascii[0], ascii[1], ascii[2], ascii[3]);
}

int httpc_dime_next(httpc_conn_t* conn, long content_length, 
                    const char *content_type, const char *id,
                    const char *dime_options, int last)
{
  int status, tmp;
  byte_t header[12];

  for (tmp=0;tmp<12;tmp++)
    header[tmp]=0;

  header[0] |= DIME_VERSION_1;

  if (conn->_dime_package_nr == 0)
    header[0] |= DIME_FIRST_PACKAGE;

  if (last)
    header[0] |= DIME_LAST_PACKAGE;

  header[1] = DIME_TYPE_URI;

  tmp = strlen(dime_options);
  header[2] = tmp >> 8;
  header[3] = tmp;

  tmp = strlen(id);
  header[4] = tmp >> 8;
  header[5] = tmp;

  tmp = strlen(content_type);
  header[6] = tmp >> 8;
  header[7] = tmp;

  header[8] = (byte_t)content_length >> 24;
  header[9] = (byte_t)content_length >> 16;
  header[10] = (byte_t)content_length >> 8;
  header[11] = (byte_t)content_length;


  _print_binary_ascii32(header[0], header[1], header[2], header[3]);
  _print_binary_ascii32(header[4], header[5], header[6], header[7]);
  _print_binary_ascii32(header[8], header[9], header[10], header[11]);

	status = http_output_stream_write(conn->out, header, 12);
	if (status != H_OK)
		return status;
  
	status = http_output_stream_write(conn->out, (const byte_t*)dime_options, strlen(dime_options));
	if (status != H_OK)
		return status;
  
	status = http_output_stream_write(conn->out, (const byte_t*)id, strlen(id));
	if (status != H_OK)
		return status;
  
	status = http_output_stream_write(conn->out, (const byte_t*)content_type, strlen(content_type));
	if (status != H_OK)
		return status;
  
  return status;
}


hresponse_t* httpc_dime_end(httpc_conn_t *conn)
{
	int             status;
	hresponse_t    *res;

   Flush put stream 
  status = http_output_stream_flush(conn->out);

  if (status != H_OK)
  {
    _httpc_set_error(conn, status, "Can not flush output stream");
    return NULL;
  }

	res = hresponse_new_from_socket(conn->sock);
	if (res == NULL)
	{
  	_httpc_set_error(conn, -1, "Can not get response ");
  	return NULL;
	}
		
	return res;
}

*/
/* ---------------------------------------------------
  MIME support functions httpc_mime_* function set
-----------------------------------------------------*/

static
void _httpc_mime_get_boundary(httpc_conn_t *conn, char *dest)
{
  sprintf(dest, "---=.Part_NH_%d", conn->id);
  log_verbose2("boundary= \"%s\"", dest);
}

herror_t httpc_mime_begin(httpc_conn_t *conn, const char *url,
  const char* related_start, 
  const char* related_start_info, 
  const char* related_type)
{
	herror_t             status;
	char            buffer[300];
	char            temp[75];
	char            boundary[75];

	/* 
		Set Content-type 
		Set multipart/related parameter
		  type=..; start=.. ; start-info= ..; boundary=...
		  
	*/
	sprintf(buffer, "multipart/related;");
	/*
	  using sprintf instead of snprintf because visual c does not support snprintf
	*/
#ifdef WIN32
#define snprintf(buffer, num, s1, s2) sprintf(buffer, s1,s2)
#endif

  if (related_type) {
    snprintf(temp, 75, " type=\"%s\";", related_type); 
    strcat(buffer, temp);
  }
  
  if (related_start) {
    snprintf(temp, 75, " start=\"%s\";", related_start); 
    strcat(buffer, temp);
  }

  if (related_start_info) {
    snprintf(temp, 75, " start-info=\"%s\";", related_start_info); 
    strcat(buffer, temp);
  }

  _httpc_mime_get_boundary(conn, boundary);
  snprintf(temp, 75, " boundary=\"%s\"", boundary);
  strcat(buffer, temp);

	httpc_set_header(conn, HEADER_CONTENT_TYPE, buffer);

	status = httpc_post_begin(conn, url);
	return status;
}


herror_t httpc_mime_next(httpc_conn_t *conn, 
  const char* content_id,
  const char* content_type, 
  const char* transfer_encoding)
{
  herror_t            status;
  char           buffer[512];
  char           boundary[75];

  /* Get the boundary string */
  _httpc_mime_get_boundary(conn, boundary);
  sprintf(buffer, "\r\n--%s\r\n", boundary);

  /* Send boundary */
  status = http_output_stream_write(conn->out, 
    (const byte_t*)buffer, strlen(buffer));

	if (status != H_OK)
		return status;

	/* Send Content header */
	sprintf(buffer, "%s: %s\r\n%s: %s\r\n%s: %s\r\n\r\n", 
	 HEADER_CONTENT_TYPE, content_type,
	 HEADER_CONTENT_TRANSFER_ENCODING, transfer_encoding,
	 HEADER_CONTENT_ID, content_id);

  status = http_output_stream_write(conn->out, 
    (const byte_t*)buffer, strlen(buffer));

	return status;
}


herror_t httpc_mime_end(httpc_conn_t *conn, hresponse_t** out)
{
  herror_t            status;
  char           buffer[512];
  char           boundary[75];

  /* Get the boundary string */
  _httpc_mime_get_boundary(conn, boundary);
  sprintf(buffer, "\r\n--%s--\r\n\r\n", boundary);

  /* Send boundary */
  status = http_output_stream_write(conn->out, 
    (const byte_t*)buffer, strlen(buffer));

  if (status != H_OK)
    return status;

  /* Flush put stream */
  status = http_output_stream_flush(conn->out);

  if (status != H_OK)
  {
    return status;
  }

	status = hresponse_new_from_socket(conn->sock, out);
	if (status != H_OK)
	{
  	return status;
	}
		
	return H_OK;

}


/**
  Send boundary and part header and continue 
  with next part
*/
herror_t
httpc_mime_send_file (httpc_conn_t * conn,
                      const char *content_id,
                      const char *content_type,
                      const char *transfer_encoding, const char *filename)
{
  herror_t status;
  FILE *fd = fopen (filename, "rb");
  byte_t buffer[MAX_FILE_BUFFER_SIZE];
  size_t size;

  if (fd == NULL)
    return herror_new("httpc_mime_send_file", FILE_ERROR_OPEN, 
	"Can not open file '%s'", filename);

  status =
    httpc_mime_next(conn, content_id, content_type, transfer_encoding);
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
      return herror_new("httpc_mime_send_file", FILE_ERROR_READ, 
			"Can not read from file '%s'", filename);
    }

	if (size>0)
	{
		/*DEBUG: fwrite(buffer, 1, size, stdout);*/
		status = http_output_stream_write (conn->out, buffer, size);
		if (status != H_OK) {
		fclose (fd);
		return status;
		}
	}
  }

  fclose (fd);
  log_verbose1("file sent!");
  return H_OK;
}


