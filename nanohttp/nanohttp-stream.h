/******************************************************************
 *  $Id: nanohttp-stream.h,v 1.1 2004/09/19 07:05:03 snowdrop Exp $
 *
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003-2004  Ferhat Ayaz
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
#ifndef NANO_HTTP_STREAM_H 
#define NANO_HTTP_STREAM_H 

#include <nanohttp/nanohttp-socket.h>
#include <nanohttp/nanohttp-common.h>

/**
  Supported transfer types
*/
typedef enum http_transfer_type
{
  HTTP_TRANSFER_CONTENT_LENGTH,
  HTTP_TRANSFER_CONTENT_CHUNKED,
  HTTP_TRANSFER_CONNECTION_CLOSE
}http_transfer_type_t;

/**
  HTTP INPUT STREAM
*/
typedef struct http_input_stream
{
  hsocket_t sock;
  http_transfer_type_t type; 
  size_t received;
  size_t content_length;
  size_t chunk_size;
}http_input_stream_t;


/**
  Creates a new input stream. 
*/
http_input_stream_t *http_input_stream_new(hsocket_t sock, hpair_t *header);

/**
  Free input stream
*/
void http_input_stream_free(http_input_stream_t *stream);

/**
  Returns the actual status of the stream.
*/
int http_input_stream_is_ready(http_input_stream_t *stream);

/**
  Returns the actual read bytes
*/
int http_input_stream_read(http_input_stream_t *stream, 
                           byte_t *dest, size_t size);

#endif






