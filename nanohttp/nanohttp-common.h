/******************************************************************
 *  $Id: nanohttp-common.h,v 1.35 2006/11/25 15:38:10 m0gg Exp $
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
#ifndef __nanohttp_common_h
#define __nanohttp_common_h

#define HEADER_CONTENT_ID		"Content-Id"
#define HEADER_CONTENT_TRANSFER_ENCODING "Content-Transfer-Encoding"
#define TRANSFER_ENCODING_CHUNKED	"chunked"

/**
 *
 * General Header Fields
 *
 * There are a few header fields which have general applicability for both
 * request and response messages, but which do not apply to the entity being
 * transferred. These header fields apply only to the message being transmitted.
 *
 * @see http://www.ietf.org/rfc/rfc/2616.txt
 *
 */

/**
 *
 * The Cache-Control general-header field is used to specify directives that MUST
 * be obeyed by all caching mechanisms along the request/response chain. The
 * directives specify behavior intended to prevent caches from adversely
 * interfering with the request or response. These directives typically override
 * the default caching algorithms. Cache directives are unidirectional in that
 * the presence of a directive in a request does not imply that the same
 * directive is to be given in the response.
 *
 */
#define HEADER_CACHE_CONTROL		"Cache-Control"

/**
 *
 * The Connection general-header field allows the sender to specify options that
 * are desired for that particular connection and MUST NOT be communicated by
 * proxies over further connections.
 *
 */
#define HEADER_CONNECTION		"Connection"

/**
 *
 * The Date general-header field represents the date and time at which the
 * message was originated, having the same semantics as orig-date in RFC 822.
 * The field value is an HTTP-date, as described in section 3.3.1; it MUST be
 * sent in RFC 1123 [8]-date format.
 *
 * @see http://www.ietf.org/rfc/rfc822.txt,
 *      http://www.ietf.org/rfc/rfc1123.txt
 *
 */
#define HEADER_DATE			"Date"

/**
 *
 * The Pragma general-header field is used to include implementation-specific
 * directives that might apply to any recipient along the request/response chain.
 * All pragma directives specify optional behavior from the viewpoint of the
 * protocol; however, some systems MAY require that behavior be consistent with
 * the directives.
 *
 */
#define HEADER_PRAGMA			"Pragma"

/**
 *
 * The Trailer general field value indicates that the given set of header fields
 * is present in the trailer of a message encoded with chunked transfer-coding.
 *
 */
#define HEADER_TRAILER			"Trailer"

/**
 *
 * The Transfer-Encoding general-header field indicates what (if any) type of
 * transformation has been applied to the message body in order to safely
 * transfer it between the sender and the recipient. This differs from the
 * content-coding in that the transfer-coding is a property of the message, not
 * of the entity.
 *
 */
#define HEADER_TRANSFER_ENCODING	"Transfer-Encoding"

/**
 *
 * The Upgrade general-header allows the client to specify what additional
 * communication protocols it supports and would like to use if the server finds
 * it appropriate to switch protocols. The server MUST use the Upgrade header
 * field within a 101 (Switching Protocols) response to indicate which
 * protocol(s) are being switched.
 *
 */
#define HEADER_UPGRADE			"Upgrade"

/**
 *
 * The Via general-header field MUST be used by gateways and proxies to indicate
 * the intermediate protocols and recipients between the user agent and the
 * server on requests, and between the origin server and the client on responses.
 * It is analogous to the "Received" field of RFC 822 and is intended to be used
 * for tracking message forwards, avoiding request loops, and identifying the
 * protocol capabilities of all senders along the request/response chain.
 *
 * @see http://www.ietf.org/rfc/rfc822.txt
 *
 */
#define HEADER_VIA			"Via"

/**
 *
 * The Warning general-header field is used to carry additional information about
 * the status or transformation of a message which might not be reflected in the
 * message. This information is typically used to warn about a possible lack of
 * semantic transparency from caching operations or transformations applied to
 * the entity body of the message.
 *
 */
#define HEADER_WARNING			"Warning"

/**
 *
 * Entity Header Fields
 *
 * Entity-header fields define metainformation about the entity-body or, if no
 * body is present, about the resource identified by the request. Some of this
 * metainformation is OPTIONAL; some might be REQUIRED by portions of this
 * specification. (see RFC2616 7.1)
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */

/**
 *
 * The Allow entity-header field lists the set of methods supported by the
 * resource identified by the Request-URI. The purpose of this field is strictly
 * to inform the recipient of valid methods associated with the resource. An
 * Allow header field MUST be present in a 405 (Method Not Allowed) response.
 *
 */
#define HEADER_ALLOW			"Allow"

/**
 *
 * The Content-Encoding entity-header field is used as a modifier to the
 * media-type. When present, its value indicates what additional content codings
 * have been applied to the entity-body, and thus what decoding mechanisms must
 * be applied in order to obtain the media-type referenced by the Content-Type
 * header field. Content-Encoding is primarily used to allow a document to be
 * compressed without losing the identity of its underlying media type.
 *
 */
#define HEADER_CONTENT_ENCODING		"Content-Encoding"

/**
 *
 * The Content-Language entity-header field describes the natural language(s) of
 * the intended audience for the enclosed entity. Note that this might not be
 * equivalent to all the languages used within the entity-body.
 *
 */
#define HEADER_CONTENT_LANGUAGE		"Content-Language"

/**
 *
 * The Content-Length entity-header field indicates the size of the entity-body,
 * in decimal number of OCTETs, sent to the recipient or, in the case of the HEAD
 * method, the size of the entity-body that would have been sent had the request
 * been a GET.
 *
 */
#define HEADER_CONTENT_LENGTH		"Content-Length"

/**
 *
 * The Content-Location entity-header field MAY be used to supply the resource
 * location for the entity enclosed in the message when that entity is accessible
 * from a location separate from the requested resource's URI. A server SHOULD
 * provide a Content-Location for the variant corresponding to the response
 * entity; especially in the case where a resource has multiple entities
 * associated with it, and those entities actually have separate locations by
 * which they might be individually accessed, the server SHOULD provide a
 * Content-Location for the particular variant which is returned.
 *
 */
#define HEADER_CONTENT_LOCATION		"Content-Location"

/**
 *
 * The Content-MD5 entity-header field, as defined in RFC 1864, is an MD5 digest
 * of the entity-body for the purpose of providing an end-to-end message
 * integrity check (MIC) of the entity-body. (Note: a MIC is good for detecting
 * accidental modification of the entity-body in transit, but is not proof
 * against malicious attacks.)
 *
 * @see http://www.ietf.org/rfc/rfc1864.txt
 *
 */
#define HEADER_CONTENT_MD5		"Content-MD5"

/**
 *
 * The Content-Range entity-header is sent with a partial entity-body to specify
 * where in the full entity-body the partial body should be applied. Range units
 * are defined in RFC 2616 section 3.12.
 *
 * @see http://www.ietf.org/rfc/rcf2616.txt
 *
 */
#define HEADER_CONTENT_RANGE		"Content-Range"

/**
 *
 * The Content-Type entity-header field indicates the media type of the
 * entity-body sent to the recipient or, in the case of the HEAD method, the
 * media type that would have been sent had the request been a GET.
 *
 */
#define HEADER_CONTENT_TYPE		"Content-Type"

/**
 *
 * The Expires entity-header field gives the date/time after which the response
 * is considered stale. A stale cache entry may not normally be returned by a
 * cache (either a proxy cache or a user agent cache) unless it is first
 * validated with the origin server (or with an intermediate cache that has a
 * fresh copy of the entity). See RFC 2616 section 13.2 for further discussion of
 * the expiration model.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
#define HEADER_EXPIRES			"Expires"

/**
 *
 * The Last-Modified entity-header field indicates the date and time at which the
 * origin server believes the variant was last modified.
 *
 */
#define HEADER_LAST_MODIFIED		"Last-Modified"

/**
 *
 * Common commandline arguments for client and server.
 *
 */
#define NHTTP_ARG_CERT		"-NHTTPcert"
#define NHTTP_ARG_CERTPASS	"-NHTTPcertpass"
#define NHTTP_ARG_CA		"-NHTTPCA"

#ifndef SAVE_STR
#define SAVE_STR(str) ((str==0)?("(null)"):(str))
#endif

#define BOUNDARY_LENGTH 18

#define MAX_HEADER_SIZE 4256
#define MAX_FILE_BUFFER_SIZE 4256

#define REQUEST_MAX_PATH_SIZE 1024
#define RESPONSE_MAX_DESC_SIZE 1024
      
#define URL_MAX_HOST_SIZE      120
#define URL_MAX_CONTEXT_SIZE  1024

/* TODO (#1#): find proper ports */
#define URL_DEFAULT_PORT_HTTP 80
#define URL_DEFAULT_PORT_HTTPS 81
#define URL_DEFAULT_PORT_FTP 120

/**
 *
 * Indicates the version of the used HTTP protocol.
 *
 */
typedef enum _http_version
{
  HTTP_1_0,
  HTTP_1_1                      /* default */
} http_version_t;


/**
 *
 * The set of common methods for HTTP/1.1 is defined below. Although this set
 * can be expanded, additional methods cannot be assumed to share the same
 * semantics for separately extended clients and servers.
 *
 * The Host request-header field MUST accompany all HTTP/1.1 requests.
 *
 * @see HTTP_HEADER_HOST
 *
 */
typedef enum _hreq_method
{
  /**
   *
   * The POST method is used to request that the origin server accept the entity
   * enclosed in the request as a new subordinate of the resource identified by
   * the Request-URI in the Request-Line. POST is designed to allow a uniform
   * method to cover the following functions:
   * - Annotation of existing resources;
   * - Posting a message to a bulletin board, newsgroup, mailing list, or
   *   similar group of articles;
   * - Providing a block of data, such as the result of submitting a form, to a
   *   data-handling process;
   * - Extending a database through an append operation.
   *
   */
 HTTP_REQUEST_POST,
  /**
   *
   * The GET method means retrieve whatever information (in the form of an entity)
   * is identified by the Request-URI. If the Request-URI refers to a
   * data-producing process, it is the produced data which shall be returned as
   * the entity in the response and not the source text of the process, unless
   * that text happens to be the output of the process.
   *
   */
  HTTP_REQUEST_GET,
  /**
   *
   * The OPTIONS method represents a request for information about the
   * communication options available on the request/response chain identified by
   * the Request-URI. This method allows the client to determine the options
   * and/or requirements associated with a resource, or the capabilities of a
   * server, without implying a resource action or initiating a resource
   * retrieval.
   *
   */
  HTTP_REQUEST_OPTIONS,
  /**
   *
   * The HEAD method is identical to GET except that the server MUST NOT return
   * a message-body in the response. The metainformation contained in the HTTP
   * headers in response to a HEAD request SHOULD be identical to the information
   * sent in response to a GET request. This method can be used for obtaining
   * metainformation about the entity implied by the request without transferring
   * the entity-body itself. This method is often used for testing hypertext
   * links for validity, accessibility, and recent modification.
   *
   */
  HTTP_REQUEST_HEAD,
  /**
   *
   * The PUT method requests that the enclosed entity be stored under the
   * supplied Request-URI. If the Request-URI refers to an already existing
   * resource, the enclosed entity SHOULD be considered as a modified version of
   * the one residing on the origin server. If the Request-URI does not point to
   * an existing resource, and that URI is capable of being defined as a new
   * resource by the requesting user agent, the origin server can create the
   * resource with that URI. If a new resource is created, the origin server MUST
   * inform the user agent via the 201 (Created) response. If an existing
   * resource is modified, either the 200 (OK) or 204 (No Content) response codes
   * SHOULD be sent to indicate successful completion of the request. If the
   * resource could not be created or modified with the Request-URI, an
   * appropriate error response SHOULD be given that reflects the nature of the
   * problem. The recipient of the entity MUST NOT ignore any Content-* (e.g.
   * Content-Range) headers that it does not understand or implement and MUST
   * return a 501 (Not Implemented) response in such cases.
   *
   */
  HTTP_REQUEST_PUT,
  /**
   *
   * The DELETE method requests that the origin server delete the resource
   * identified by the Request-URI. This method MAY be overridden by human
   * intervention (or other means) on the origin server. The client cannot be
   * guaranteed that the operation has been carried out, even if the status code
   * returned from the origin server indicates that the action has been completed
   * successfully. However, the server SHOULD NOT indicate success unless, at the
   * time the response is given, it intends to delete the resource or move it to
   * an inaccessible location.
   *
   */
  HTTP_REQUEST_DELETE,
  /**
   *
   * The TRACE method is used to invoke a remote, application-layer loop-back of
   * the request message. The final recipient of the request SHOULD reflect the
   * message received back to the client as the entity-body of a 200 (OK)
   * response. The final recipient is either the origin server or the first proxy
   * or gateway to receive a Max-Forwards value of zero (0) in the request (see
   * section 14.31). A TRACE request MUST NOT include an entity.
   *
   */
  HTTP_REQUEST_TRACE,
  /**
   *
   * This specification reserves the method name CONNECT for use with a proxy
   * that can dynamically switch to being a tunnel (e.g. SSL tunneling [44]).
   *
   */
  HTTP_REQUEST_CONNECT,
  HTTP_REQUEST_UNKOWN
} hreq_method_t;

/**
 *
 * hpairnode_t represents a pair (key, value) pair. This is also a linked list.
 *
 */
typedef struct hpair hpair_t;
struct hpair
{
  char *key;
  char *value;
  hpair_t *next;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Creates a new pair with the given parameters. Both strings key and value will
 * be cloned while creating the pair. 
 *
 * @param key the key of the (key,value) pair
 * @param value the value of the (key,value) pair
 * @param next next pair node in the linked list
 *
 * @returns A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *          to free the pair.
 *
 */
extern hpair_t *hpairnode_new(const char *key, const char *value, hpair_t * next);

/**
 *
 * Creates a new pair from a given string. This function will split 'str' with
 * the found first delimiter 'delim'. The 'value' field  of the newly created
 * pair will have the value "", if no delimiter was found/ Whitespaces (' ') will
 * be removed from the beginnig of the parsed value.
 *
 * @param str A string to parse
 * @param delim a delimiter to use while splitting into key,value
 * @param next next pair node in the linked list
 *
 * @returns A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *          to free the pair.
 *
 */
extern hpair_t *hpairnode_parse(const char *str, const char *delim, hpair_t * next);

/**
 *
 * Frees a given pair.
 *
 * @param pair the pair to free
 *
 */
extern void hpairnode_free(hpair_t * pair);

/**
 *
 * Makes a deep free operation. All pairnodes, beginning with the given pari, in
 * the linked list will be destroyed. 
 *
 * @param pair the pair to start to free the linked list
 *
 */
extern void hpairnode_free_deep(hpair_t * pair);

/**
 *
 * Returns the (key,value) pair, which key is the given 'key'.
 *
 * @param pair the first pair to start to search from.
 * @param key key to find the in the pair.
 * @returns if a value will be found, this function will 
 *
 * @return the value (do not free this string) or NULL if no pair was found with
 *         the key 'key'.
 *
 */
extern char *hpairnode_get(hpair_t * pair, const char *key);

/**
 *
 * Returns the (key,value) pair, which key is the given 'key'. The case will be
 * ignored while comparing the key strings.
 *
 * @param pair the first pair to start to search from.
 * @param key key to find the in the pair.
 * @returns if a value will be found, this function will 
 *
 * @return the value (do not free this string) or NULL if no pair was found with
 *         the key 'key'.
 *
 */
extern char *hpairnode_get_ignore_case(hpair_t * pair, const char *key);

/**
 *
 * This function will create a new pair and fills the (key,value) fields of a
 * given pair. Note that the 'next' field will not be copied.
 *
 * @param src the source pair object to copy.
 *
 * @returns a newly created pair with the same (key,value) pairs as in 'src'.
 *          This fields will be cloned. The'next' field will be set to NULL.
 *
 * @see hpairnode_copy_deep
 *
 */
extern hpair_t *hpairnode_copy(const hpair_t * src);

/**
 *
 * Clones the hole linked list. 
 *
 * @param src the source pair object to copy from
 *
 * @return the first object in the linked list. 
 *
 * @see hpairnode_copy
 *
 */
extern hpair_t *hpairnode_copy_deep(const hpair_t * src);

/**
 *
 * Debug functions
 *
 */
extern void hpairnode_dump_deep(hpair_t * pair);
extern void hpairnode_dump(hpair_t * pair);

/**
 *
 * The protocol types in enumeration format. Used in some other nanohttp objects
 * like hurl_t.
 *
 * @see hurl_t
 *
 */
typedef enum _hprotocol
{
  PROTOCOL_HTTP,
  PROTOCOL_HTTPS,
  PROTOCOL_FTP
} hprotocol_t;



/**
 *
 * The URL object. A representation of an URL like:
 *
 * [protocol]://[host]:[port]/[context]
 *
 * @see http://www.ietf.org/rfc/rfc2396.txt
 *
 */
typedef struct _hurl
{
  /**
    The transfer protocol. 
    Note that only PROTOCOL_HTTP is supported by nanohttp.
  */
  hprotocol_t protocol;

  /**
    The port number. If no port number was given in the URL,
    one of the default port numbers will be selected. 
    URL_HTTP_DEFAULT_PORT    
    URL_HTTPS_DEFAULT_PORT   
    URL_FTP_DEFAULT_PORT    
  */
  int port;

  /** The hostname */
  char host[URL_MAX_HOST_SIZE];

  /** The string after the hostname. */
  char context[URL_MAX_CONTEXT_SIZE];
} hurl_t;

/**
 *
 * Parses the given 'urlstr' and fills the given hurl_t object.
 *
 * @param obj the destination URL object to fill
 * @param url the URL in string format
 *
 * @returns H_OK on success or one of the following otherwise
 *          - URL_ERROR_UNKNOWN_PROTOCOL 
 *          - URL_ERROR_NO_PROTOCOL 
 *          - URL_ERROR_NO_HOST 
 *
 */
extern herror_t hurl_parse(hurl_t * obj, const char *url);

/**
 *
 * Object representation of the content-type field in a HTTP header: 
 *
 * Example:
 *
 * text/xml; key="value" key2="value2' ...
 *
 */
typedef struct _content_type
{
  char type[128];
  hpair_t *params;
} content_type_t;

/**
 *
 * Parses the given string and creates a new ccontent_type_t object. 
 *
 * @param content_type_str the string representation of the content-type field in
 *        a HTTP header.
 *
 * @return A newly created content_type_t object. Free this object with
 *         content_type_free();
 *
 * @see content_type_free
 */
extern content_type_t *content_type_new(const char *content_type_str);


/**
 *
 * Frees the given content_type_t object
 *
 */
extern void content_type_free(content_type_t * ct);

/**
 *
 * part. Attachment
 *
 */
struct part_t
{
  char id[250];
  char location[250];
  hpair_t *header;
  char content_type[128];
  char transfer_encoding[128];
  char filename[250];
  struct part_t *next;
  int deleteOnExit;             /* default is 0 */
};

extern struct part_t *part_new(const char *id, const char *filename, const char *content_type, const char *transfer_encoding, struct part_t * next);

extern void part_free(struct part_t * part);

/**
 *
 * Attachments
 *
 */
struct attachments_t
{
  struct part_t *parts;
  struct part_t *last;
  struct part_t *root_part;
};

extern struct attachments_t *attachments_new(void);       /* should be used internally */

/**
 *
 * Free a attachment. Create attachments with MIME and DIME (DIME is not
 * supported yet).
 *
 * @see mime_get_attachments
 *
 */
extern void attachments_free(struct attachments_t * message);
extern void attachments_add_part(struct attachments_t * attachments, struct part_t * part);

#ifdef __cplusplus
}
#endif

#endif
