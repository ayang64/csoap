/******************************************************************
 *  $Id: nanohttp-request.h,v 1.11 2006/11/25 15:06:58 m0gg Exp $
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
 * programming language method invocation.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */

/**
 *
 * The Accept request-header field can be used to specify certain media types
 * which are acceptable for the response. Accept headers can be used to indicate
 * that the request is specifically limited to a small set of desired types, as
 * in the case of a request for an in-line image.
 *
 */
#define HEADER_ACCEPT			"Accept"

/**
 *
 * The Accept-Charset request-header field can be used to indicate what character
 * sets are acceptable for the response. This field allows clients capable of
 * understanding more comprehensive or special-purpose character sets to signal
 * that capability to a server which is capable of representing documents in
 * those character sets.
 *
 */
#define HEADER_ACCEPT_CHARSET		"Accept-Charset"

/**
 *
 * The Accept-Encoding request-header field is similar to Accept, but restricts
 * the content-codings (section 3.5) that are acceptable in the response.
 *
 */
#define HEADER_ACCEPT_ENCODING		"Accept-Encoding"

/**
 *
 * The Accept-Language request-header field is similar to Accept, but restricts
 * the set of natural languages that are preferred as a response to the request.
 *
 */
#define HEADER_ACCEPT_LANGUAGE		"Accept-Language"

/**
 *
 * A user agent that wishes to authenticate itself with a server -- usually, but
 * not necessarily, after receiving a 401 response -- does so by including an
 * Authorization request-header field with the request. The Authorization field
 * value consists of credentials containing the authentication information of the
 * user agent for the realm of the resource being requested.
 *
 */
#define HEADER_AUTHORIZATION		"Authorization"

/**
 *
 * The Expect request-header field is used to indicate that particular server
 * behaviors are required by the client.
 *
 */
#define HEADER_EXPECT			"Expect"

/**
 *
 * The From request-header field, if given, SHOULD contain an Internet e-mail
 * address for the human user who controls the requesting user agent. The
 * address SHOULD be machine-usable, as defined by "mailbox" in RFC 822 as
 * updated by RFC 1123.
 *
 * @see http://www.ietf.org/rfc/rfc1123.txt,
 *      http://www.ietf.org/rfc/rfc822.txt
 *
 */
#define HEADER_FROM			"From"

/**
 *
 * The Host request-header field specifies the Internet host and port number of
 * the resource being requested, as obtained from the original URI given by the
 * user or referring resource (generally an HTTP URL, as described in section
 * 3.2.2). The Host field value MUST represent the naming authority of the origin
 * server or gateway given by the original URL. This allows the origin server or
 * gateway to differentiate between internally-ambiguous URLs, such as the root
 * "/" URL of a server for multiple host names on a single IP address.
 *
 */
#define HEADER_HOST			"Host"

/**
 *
 * The If-Match request-header field is used with a method to make it
 * conditional. A client that has one or more entities previously obtained from
 * the resource can verify that one of those entities is current by including a
 * list of their associated entity tags in the If-Match header field. Entity tags
 * are defined in section 3.11. The purpose of this feature is to allow efficient
 * updates of cached information with a minimum amount of transaction overhead.
 * It is also used, on updating requests, to prevent inadvertent modification of
 * the wrong version of a resource. As a special case, the value "*" matches any
 * current entity of the resource.
 *
 */
#define HEADER_IF_MATCH			"If-Match"

/**
 *
 * The If-Modified-Since request-header field is used with a method to make it
 * conditional: if the requested variant has not been modified since the time
 * specified in this field, an entity will not be returned from the server;
 * instead, a 304 (not modified) response will be returned without any
 * message-body.
 *
 */
#define HEADER_IF_MODIFIED_SINCE	"If-Modified-Since"

/**
 *
 * The If-None-Match request-header field is used with a method to make it
 * conditional. A client that has one or more entities previously obtained from
 * the resource can verify that none of those entities is current by including a
 * list of their associated entity tags in the If-None-Match header field. The
 * purpose of this feature is to allow efficient updates of cached information
 * with a minimum amount of transaction overhead. It is also used to prevent a
 * method (e.g. PUT) from inadvertently modifying an existing resource when the
 * client believes that the resource does not exist.
 *
 */
#define HEADER_IF_NONE_MATCH		"If-None-Match"

/**
 *
 * If a client has a partial copy of an entity in its cache, and wishes to have
 * an up-to-date copy of the entire entity in its cache, it could use the Range
 * request-header with a conditional GET (using either or both of
 * If-Unmodified-Since and If-Match.) However, if the condition fails because the
 * entity has been modified, the client would then have to make a second request
 * to obtain the entire current entity-body.
 *
 */
#define HEADER_IF_RANGE			"If-Range"

/**
 *
 * The If-Unmodified-Since request-header field is used with a method to make it
 * conditional. If the requested resource has not been modified since the time
 * specified in this field, the server SHOULD perform the requested operation as
 * if the If-Unmodified-Since header were not present.
 *
 */
#define HEADER_IF_UNMODIFIED_SINCE	"If-Unmodified-Since"

/**
 *
 * The Max-Forwards request-header field provides a mechanism with the TRACE
 * (section 9.8) and OPTIONS (section 9.2) methods to limit the number of
 * proxies or gateways that can forward the request to the next inbound server.
 * This can be useful when the client is attempting to trace a request chain
 * which appears to be failing or looping in mid-chain.
 *
 */
#define HEADER_IF_MAX_FORWARDS		"Max-Forwards"

/**
 *
 * The Proxy-Authorization request-header field allows the client to identify
 * itself (or its user) to a proxy which requires authentication. The
 * Proxy-Authorization field value consists of credentials containing the
 * authentication information of the user agent for the proxy and/or realm of
 * the resource being requested.
 *
 */
#define HEADER_PROXY_AUTHORIZATION	"Proxy-Authorization"

/**
 *
 * HTTP retrieval requests using conditional or unconditional GET methods MAY
 * request one or more sub-ranges of the entity, instead of the entire entity,
 * using the Range request header, which applies to the entity returned as the
 * result of the request
 *
 */
#define HEADER_RANGE			"Range"

/**
 *
 * The Referer[sic] request-header field allows the client to specify, for the
 * server's benefit, the address (URI) of the resource from which the Request-URI
 * was obtained (the "referrer", although the header field is misspelled.) The
 * Referer request-header allows a server to generate lists of back-links to
 * resources for interest, logging, optimized caching, etc. It also allows
 * obsolete or mistyped links to be traced for maintenance. The Referer field
 * MUST NOT be sent if the Request-URI was obtained from a source that does not
 * have its own URI, such as input from the user keyboard.
 *
 */
#define HEADER_REFERER			"Referer"

/**
 *
 * The TE request-header field indicates what extension transfer-codings it is
 * willing to accept in the response and whether or not it is willing to accept
 * trailer fields in a chunked transfer-coding. Its value may consist of the
 * keyword "trailers" and/or a comma-separated list of extension transfer-coding
 * names with optional accept parameters.
 *
 */
#define HEADER_TRANSFER_EXTENSION	"TE"

/**
 *
 * The User-Agent request-header field contains information about the user agent
 * originating the request. This is for statistical purposes, the tracing of
 * protocol violations, and automated recognition of user agents for the sake of
 * tailoring responses to avoid particular user agent limitations. User agents
 * SHOULD include this field with requests. The field can contain multiple
 * product tokens (section 3.8) and comments identifying the agent and any
 * subproducts which form a significant part of the user agent. By convention,
 * the product tokens are listed in order of their significance for identifying
 * the application.
 *
 */
#define HEADER_USER_AGENT		"User-Agent"

#ifdef __NHTTP_INTERNAL
struct request_statistics
{
  unsigned long bytes_transmitted;
  unsigned long bytes_received;
  struct timeval time;
};
#endif

/**
 *
 * HTTP request object
 *
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
  struct attachments_t *attachments;
  char root_part_id[150];
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NHTTP_INTERNAL
extern herror_t hrequest_new_from_socket(struct hsocket_t *sock, struct hrequest_t **out);
extern void hrequest_free(struct hrequest_t *req);
#endif

#ifdef __cplusplus
}
#endif

#endif
