/******************************************************************
 *  $Id: nanohttp-response.h,v 1.11 2006/11/26 20:13:06 m0gg Exp $
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

/** @file
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

/**
 *
 * The Accept-Ranges response-header field allows the server to indicate its
 * acceptance of range requests for a resource
 *
 */
#define HEADER_ACCEPT_RANGES		"Accept-Ranges"

/**
 *
 * The Age response-header field conveys the sender's estimate of the amount of
 * time since the response (or its revalidation) was generated at the origin
 * server. A cached response is "fresh" if its age does not exceed its freshness
 * lifetime.
 *
 */
#define HEADER_AGE			"Age"

/**
 *
 * The ETag response-header field provides the current value of the entity tag
 * for the requested variant. The headers used with entity tags are described in
 * RFC 2616 sections 14.24, 14.26 and 14.44. The entity tag MAY be used for
 * comparison with other entities from the same resource
 *
 * @see http://www.ietf.org/rfc/rfc/2616.txt
 *
 */
#define HEADER_EXTENSION_TAG		"ETag"

/**
 *
 * The Location response-header field is used to redirect the recipient to a
 * location other than the Request-URI for completion of the request or
 * identification of a new resource. For 201 (Created) responses, the Location
 * is that of the new resource which was created by the request. For 3xx
 * responses, the location SHOULD indicate the server's preferred URI for
 * automatic redirection to the resource. The field value consists of a single
 * absolute URI.
 *
 */
#define HEADER_LOCATION			"Location"

/**
 *
 * The Proxy-Authenticate response-header field MUST be included as part of a
 * 407 (Proxy Authentication Required) response. The field value consists of a
 * challenge that indicates the authentication scheme and parameters applicable
 * to the proxy for this Request-URI.
 *
 */
#define HEADER_PROXY_AUTHENTICATE	"Proxy-Authenticate"

/**
 *
 * The Retry-After response-header field can be used with a 503 (Service
 * Unavailable) response to indicate how long the service is expected to be
 * unavailable to the requesting client. This field MAY also be used with any
 * 3xx (Redirection) response to indicate the minimum time the user-agent is
 * asked wait before issuing the redirected request. The value of this field
 * can be either an HTTP-date or an integer number of seconds (in decimal) after
 * the time of the response.
 *
 */
#define HEADER_RETRY_AFTER		"Retry-After"

/**
 *
 * The Server response-header field contains information about the software used
 * by the origin server to handle the request. The field can contain multiple
 * product tokens (section 3.8) and comments identifying the server and any
 * significant subproducts. The product tokens are listed in order of their
 * significance for identifying the application.
 *
 */
#define HEADER_SERVER			"Server"

/**
 *
 * The Vary field value indicates the set of request-header fields that fully
 * determines, while the response is fresh, whether a cache is permitted to use
 * the response to reply to a subsequent request without revalidation. For
 * uncacheable or stale responses, the Vary field value advises the user agent
 * about the criteria that were used to select the representation. A Vary field
 * value of "*" implies that a cache cannot determine from the request headers of
 * a subsequent request whether this response is the appropriate representation.
 * See RFC 2616 section 13.6 for use of the Vary header field by caches.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
#define HEADER_VARY			"Vary"

/**
 *
 * The WWW-Authenticate response-header field MUST be included in 401
 * (Unauthorized) response messages. The field value consists of at least one
 * challenge that indicates the authentication scheme(s) and parameters
 * applicable to the Request-URI.
 *
 *   WWW-Authenticate  = "WWW-Authenticate" ":" 1#challenge
 *
 * The HTTP access authentication process is described in "HTTP Authentication:
 * Basic and Digest Access Authentication" [43]. User agents are advised to take
 * special care in parsing the WWW-Authenticate field value as it might contain
 * more than one challenge, or if more than one WWW-Authenticate header field is
 * provided, the contents of a challenge itself can contain a comma-separated
 * list of authentication parameters.
 *
 * @see http://www.ietf.org/rfc/rfc2617.txt
 *
 */
#define HEADER_WWW_AUTHENTICATE		"WWW-Authenticate"

/**
 *
 * HTTP response object
 *
 */
typedef struct hresponse
{
  http_version_t version;
  int errcode;
  char desc[RESPONSE_MAX_DESC_SIZE];

  hpair_t *header;

  struct http_input_stream_t *in;
  content_type_t *content_type;
  struct attachments_t *attachments;
  char root_part_id[150];
} hresponse_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NHTTP_INTERNAL
extern herror_t hresponse_new_from_socket(struct hsocket_t *sock, hresponse_t **out);
#endif

extern void hresponse_free(hresponse_t * res);

#ifdef __cplusplus
}
#endif

#endif
