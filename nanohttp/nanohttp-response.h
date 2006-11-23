/******************************************************************
 *  $Id: nanohttp-response.h,v 1.9 2006/11/23 15:27:33 m0gg Exp $
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
#ifndef __nanohttp_response_h
#define __nanohttp_response_h

/**
 *
 * Response Header Fields
 *
 * The response-header fields allow the server to pass additional information
 * about the response which cannot be placed in the Status-Line. These header
 * fields give information about the server and about further access to the
 * resource identified by the Request-URI. (see RFC2616)
 *
 * @see	http://www.ietf.org/rfc/rfc2616.txt
 *
 */
#define HEADER_ACCEPT_RANGES		"Accept-Ranges"
#define HEADER_AGE			"Age"
#define HEADER_EXTENSION_TAG		"ETag"
#define HEADER_LOCATION			"Location"
#define HEADER_PROXY_AUTHENTICATE	"Proxy-Authenticate"
#define HEADER_RETRY_AFTER		"Retry-After"
#define HEADER_SERVER			"Server"
#define HEADER_VARY			"Vary"
#define HEADER_WWW_AUTHENTICATE		"WWW-Authenticate"

/* response object */
typedef struct hresponse
{
  http_version_t version;
  int errcode;
  char desc[RESPONSE_MAX_DESC_SIZE];

  hpair_t *header;

  struct http_input_stream_t *in;
  content_type_t *content_type;
  attachments_t *attachments;
  char root_part_id[150];
} hresponse_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NHTTP_INTERNAL
herror_t hresponse_new_from_socket(struct hsocket_t *sock, hresponse_t ** out);
void hresponse_free(hresponse_t * res);
#endif

#ifdef __cplusplus
}
#endif

#endif
