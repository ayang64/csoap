/******************************************************************
 *  $Id: nanohttp-request.h,v 1.10 2006/11/23 15:27:33 m0gg Exp $
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
#ifndef __nhttp_request_h
#define __nhttp_request_h

/**
 *
 * Request Header Fields
 *
 * The request-header fields allow the client to pass additional information
 * about the request, and about the client itself, to the server. These fields
 * act as request modifiers, with semantics equivalent to the parameters on a
 * programming language method invocation (see RFC2616).
 *
 */
#define HEADER_ACCEPT			"Accept"
#define HEADER_CHARSET			"Accept-Charset"
#define HEADER_ACCEPT_ENCODING		"Accept-Encoding"
#define HEADER_ACCEPT_LANGUAGE		"Accept-Language"
#define HEADER_AUTHORIZATION		"Authorization"
#define HEADER_EXPECT			"Expect"
#define HEADER_FROM			"From"
#define HEADER_HOST			"Host"
#define HEADER_IF_MATCH			"If-Match"
#define HEADER_IF_MODIFIED_SINCE	"If-Modified-Since"
#define HEADER_IF_NONE_MATCH		"If-None-Match"
#define HEADER_IF_RANGE			"If-Range"
#define HEADER_IF_UNMODIFIED_SINCE	"If-Unmodified-Since"
#define HEADER_IF_MAX_FORWARDS		"Max-Forwards"
#define HEADER_PROXY_AUTHORIZATION	"Proxy-Authorization"
#define HEADER_RANGE			"Range"
#define HEADER_REFERER			"Referer"
#define HEADER_TRANSFER_EXTENSION	"TE"
#define HEADER_USER_AGENT		"User-Agent"

#ifdef __NHTTP_INTERNAL
struct request_statistics
{
  unsigned long bytes_transmitted;
  unsigned long bytes_received;
  struct timeval time;
};
#endif

/*
  request object
 */
struct hrequest_t
{
  hreq_method_t method;
  http_version_t version;
  char path[REQUEST_MAX_PATH_SIZE];

  hpair_t *query;
  hpair_t *header;

  struct request_statistics *statistics;

  struct http_input_stream_t *in;
  content_type_t *content_type;
  attachments_t *attachments;
  char root_part_id[150];
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NHTTP_INTERNAL
herror_t hrequest_new_from_socket(struct hsocket_t *sock, struct hrequest_t **out);
void hrequest_free(struct hrequest_t *req);
#endif

#ifdef __cplusplus
}
#endif

#endif
