/******************************************************************
*  $Id: nanohttp-stream.c,v 1.1 2004/09/19 07:05:03 snowdrop Exp $
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

#define STREAM_INVALID -1
#define STREAM_INVALID_TYPE -2
#define STREAM_SOCKET_ERROR -3
#define STREAM_NO_CHUNK_SIZE -4

#include <nanohttp/nanohttp-stream.h>

static
int _http_stream_is_content_length(hpair_t *header)
{
  return 
    hpairnode_get_ignore_case(header, HEADER_CONTENT_LENGTH) != NULL;
}

static
int _http_stream_is_chunked(hpair_t *header)
{
  char *chunked;
  chunked = hpairnode_get_ignore_case(header, HEADER_TRANSFER_ENCODING);
  if (chunked != NULL)
  {
      if (!strcmp(chunked, TRANSFER_ENCODING_CHUNKED))
      {
        return 1;
      }
  }

  return 0;
}

/**
  Creates a new input stream. 
*/
http_input_stream_t *http_input_stream_new(hsocket_t sock, hpair_t *header)
{
  http_input_stream_t *result;
  char                *content_length;
  char                *chunked;

  /* Paranoya check */
  if (header == NULL)
    return NULL;

  /* Create object */
  result = (http_input_stream_t*)malloc(sizeof(http_input_stream_t));
  result->sock = sock;

  /* Find connection type */
  
  /* Check if Content-type */
  if (_http_stream_is_content_length(header))
  {
    log_verbose1("Stream transfer with 'Content-length'");
    content_length = hpairnode_get_ignore_case(header, HEADER_CONTENT_LENGTH);
    result->content_length = atoi(content_length);
    result->received = 0;
    result->type =   HTTP_TRANSFER_CONTENT_LENGTH;
  }
  /* Check if Chunked */
  else if (_http_stream_is_chunked(header))
  {
    log_verbose1("Stream transfer with 'chunked'");
    result->type = HTTP_TRANSFER_CONTENT_CHUNKED;
    result->chunk_size = -1;
    result->received = -1;
  }
  /* Assume connection close */
  else
  {
    log_verbose1("Stream transfer with 'Connection: close'");
    result->type = HTTP_TRANSFER_CONNECTION_CLOSE;
  }

  return result;
}


/**
  Free input stream
*/
void http_input_stream_free(http_input_stream_t *stream)
{
  free(stream);
}

static 
int _http_input_stream_is_content_length_ready(
  http_input_stream_t *stream)
{
  return (stream->content_length > stream->received);
}

static 
int _http_input_stream_is_chunked_ready(
  http_input_stream_t *stream)
{
  return stream->chunk_size != 0;

}

static 
int _http_input_stream_is_connection_closed_ready(
  http_input_stream_t *stream)
{
/* TODO (#1#): implement */
}

static 
int _http_input_stream_content_length_read(
  http_input_stream_t *stream, byte_t *dest, size_t size)
{
  int status;

  /* check limit */
  if (stream->content_length - stream->received < size)
    size = stream->content_length - stream->received;

  /* read from socket */
  status = hsocket_read(stream->sock, dest, size, 1);
  if (status == -1)
    return STREAM_SOCKET_ERROR;

  stream->received += status;
  return status;
}

static 
int _http_input_stream_chunked_read_chunk_size(
  http_input_stream_t *stream)
{
  char  chunk[25];
  int chunk_size, status, i = 0;
  
  while (1)
  {
    status = hsocket_read(stream->sock, &(chunk[i]), 1, 1);

    if (status == -1)
        return STREAM_SOCKET_ERROR;

    if (chunk[i] == '\r' || chunk[i] == ';')
    {
        chunk[i] = '\0';
    }
    else if (chunk[i] == '\n')
    {
        chunk[i] = '\0'; /* double check*/
  			chunk_size = strtol(chunk, (char **) NULL, 16);	/* hex to dec */
  			log_debug3("chunk_size: '%s' as dec: '%d'", chunk, chunk_size);
  			return chunk_size;
    }
    
    if (i == 24)
        return STREAM_NO_CHUNK_SIZE;
    else
        i++;
  }

  /* this should never happens */
  return STREAM_NO_CHUNK_SIZE; 
}

static 
int _http_input_stream_chunked_read(
  http_input_stream_t *stream, byte_t *dest, size_t size)
{
  int status;
  int remain, read=0;

  while (size > 0)
  {
    remain = stream->chunk_size - stream->received ;

    if (remain == 0)
    {
      /* receive new chunk size */
      stream->chunk_size = 
            _http_input_stream_chunked_read_chunk_size(stream);

      if (stream->chunk_size < 0)
      {
        /* TODO (#1#): set error flag */
        return stream->chunk_size;
      }
      else if (stream->chunk_size == 0)
      {
        return read;
      }
      remain = stream->chunk_size;
    }

    /* show remaining chunk size in socket */
    if (remain < size)
    {
      /* read from socket */
      status = hsocket_read(stream->sock, &(dest[read]), remain, 1);
      if (status == -1)
        return STREAM_SOCKET_ERROR;

    }
    else
    {
      /* read from socket */
      status = hsocket_read(stream->sock, &(dest[read]), size, 1);
      if (status == -1)
        return STREAM_SOCKET_ERROR;
    }

    read += status;
    size -= status;
    stream->received += status;
  }
}

static 
int _http_input_stream_connection_closed_read(
  http_input_stream_t *stream, byte_t *dest, size_t size)
{
/* TODO (#1#): implement */
  return 0;
}

/**
  Returns the actual status of the stream.
*/
int http_input_stream_is_ready(http_input_stream_t *stream)
{
  /* paranoya check */
  if (stream == NULL)
    return 0;

  switch (stream->type)
  {
    case HTTP_TRANSFER_CONTENT_LENGTH:
        return _http_input_stream_is_content_length_ready(stream);
    case HTTP_TRANSFER_CONTENT_CHUNKED:
        return _http_input_stream_is_chunked_ready(stream);
    case HTTP_TRANSFER_CONNECTION_CLOSE:
        return _http_input_stream_is_connection_closed_ready(stream);
    default:
        return 0;
  }
}

/**
  Returns the actual read bytes
  <0 on error
*/
int http_input_stream_read(http_input_stream_t *stream, 
                           byte_t *dest, size_t size)
{
  /* paranoya check */
  if (stream == NULL)
    return STREAM_INVALID;

  switch (stream->type)
  {
    case HTTP_TRANSFER_CONTENT_LENGTH:
        return _http_input_stream_content_length_read(stream, dest, size);
    case HTTP_TRANSFER_CONTENT_CHUNKED:
        return _http_input_stream_chunked_read(stream, dest, size);
    case HTTP_TRANSFER_CONNECTION_CLOSE:
        return _http_input_stream_connection_closed_read(stream, dest, size);
    default:
        return STREAM_INVALID_TYPE;
  }
}

