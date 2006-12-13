/******************************************************************
 *  $Id: nanohttp-common.h,v 1.47 2006/12/13 08:36:53 m0gg Exp $
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

/** @defgroup general_header_fields General Header Fields
 *
 * There are a few header fields which have general applicability for both
 * request and response messages, but which do not apply to the entity being
 * transferred. These header fields apply only to the General-header field names
 * can be extended reliably only in combination with a change in the protocol
 * version. However, new or experimental header fields may be given the semantics
 * of general header fields if all parties in the communication recognize them to
 * be general-header fields. Unrecognized header fields are treated as
 * entity-header fields.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
/*@{*/

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
 * The field value is an HTTP-date, as described in RDF 2616 section 3.3.1; it
 * MUST be sent in RFC 1123 date format.
 *
 * @see http://www.ietf.org/rfc/rfc822.txt
 * @see http://www.ietf.org/rfc/rfc1123.txt
 * @see http://www.ietf.org/rfc/rfc2616.txt
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
 * specification. (see RFC 2616 section 7.1)
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
 * @see HEADER_CONTENT_TYPE
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
 * @see HTTP_REQUEST_GET
 * @see HTTP_REQUEST_HEAD
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
 * integrity check (MIC) of the entity-body.
 *
 * Note: a MIC is good for detecting accidental modification of the entity-body
 * in transit, but is not proof against malicious attacks.
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

/*@}*/

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
 * Object representation of the content-type field in a HTTP header: 
 *
 * Example:
 *
 * @code
 * Content-Type: text/xml; key="value" key2="value2' ...
 * @endcode
 *
 * @see HEADER_CONTENT_TYPE
 *
 */
typedef struct _content_type
{
  char type[128];
  hpair_t *params;
} content_type_t;

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

/** @defgroup http_status_codes HTTP status-codes
 *
 * The Status-Code element is a 3-digit integer result code of the attempt to
 * understand and satisfy the request. These codes are fully defined in section
 * 10. The Reason-Phrase is intended to give a short textual description of the
 * Status-Code. The Status-Code is intended for use by automata and the
 * Reason-Phrase is intended for the human user. The client is not required to
 * examine or display the Reason- Phrase. The first digit of the Status-Code
 * defines the class of response. The last two digits do not have any
 * categorization role. There are 5 values for the first digit: 
 *
 * - @ref status_informational_sec - Request received, continuing process
 * - @ref status_successful_sec - The action was successfully received,
 *        understood, and accepted 
 * - @ref status_redirection_sec - Further action must be taken in order to
 *        complete the request
 * - @ref status_client_error_sec - The request contains bad syntax or cannot be
 *        fulfilled
 * - @ref status_server_error_sec - The server failed to fulfill an apparently
 *        valid request
 *
 * The individual values of the numeric status codes defined for HTTP/1.1, and an
 * example set of corresponding Reason-Phrase's, are presented below. The reason
 * phrases listed here are only recommendations -- they MAY be replaced by local
 * equivalents without affecting the protocol.
 *
 * HTTP status codes are extensible. HTTP applications are not required to
 * understand the meaning of all registered status codes, though such
 * understanding is obviously desirable. However, applications MUST understand
 * the class of any status code, as indicated by the first digit, and treat any
 * unrecognized response as being equivalent to the x00 status code of that class,
 * with the exception that an unrecognized response MUST NOT be cached. For
 * example, if an unrecognized status code of 431 is received by the client, it
 * can safely assume that there was something wrong with its request and treat
 * the response as if it had received a 400 status code. In such cases, user
 * agents SHOULD present to the user the entity returned with the response,
 * since that entity is likely to include human-readable information which will
 * explain the unusual status.
 *
 * @section status_informational_sec 1xx Informational
 *
 * This class of status code indicates a provisional response, consisting only
 * of the Status-Line and optional headers, and is terminated by an empty line.
 * There are no required headers for this class of status code. Since HTTP/1.0
 * did not define any 1xx status codes, servers MUST NOT send a 1xx response to
 * an HTTP/1.0 client except under experimental conditions.
 *
 * A client MUST be prepared to accept one or more 1xx status responses prior to
 * a regular response, even if the client does not expect a 100 (Continue)
 * status message. Unexpected 1xx status responses MAY be ignored by a user
 * agent.
 *
 * Proxies MUST forward 1xx responses, unless the connection between the proxy
 * and its client has been closed, or unless the proxy itself requested the
 * generation of the 1xx response. (For example, if a proxy adds a
 * "Expect: 100-continue" field when it forwards a request, then it need not
 * forward the corresponding 100 (Continue) response(s).)
 *
 * @section status_successful_sec 2xx Successful
 *
 * This class of status code indicates that the client's request was
 * successfully received, understood, and accepted.
 *
 * @section status_redirection_sec 3xx Redirection
 *
 * This class of status code indicates that further action needs to be taken by
 * the user agent in order to fulfill the request.  The action required MAY be
 * carried out by the user agent without interaction with the user if and only
 * if the method used in the second request is GET or HEAD. A client SHOULD
 * detect infinite redirection loops, since such loops generate network traffic
 * for each redirection.
 *
 * @section status_client_error_sec 4xx Client Error
 *
 * The 4xx class of status code is intended for cases in which the client seems
 * to have erred. Except when responding to a HEAD request, the server SHOULD
 * include an entity containing an explanation of the error situation, and
 * whether it is a temporary or permanent condition. These status codes are
 * applicable to any request method. User agents SHOULD display any included
 * entity to the user.
 *
 * If the client is sending data, a server implementation using TCP SHOULD be
 * careful to ensure that the client acknowledges receipt of the packet(s)
 * containing the response, before the server closes the input connection. If
 * the client continues sending data to the server after the close, the server's
 * TCP stack will send a reset packet to the client, which may erase the client's
 * unacknowledged input buffers before they can be read and interpreted by the
 * HTTP application.
 *
 * @section status_server_error_sec 5xx Server Error
 *
 * Response status codes beginning with the digit "5" indicate cases in which
 * the server is aware that it has erred or is incapable of performing the
 * request. Except when responding to a HEAD request, the server SHOULD include
 * an entity containing an explanation of the error situation, and whether it is
 * a temporary or permanent condition. User agents SHOULD display any included
 * entity to the user. These response codes are applicable to any request method.
 *
 * Each Status-Code is described below, including a description of which
 * method(s) it can follow and any metainformation required in the response.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
/*@{*/

/**
 *
 * The client SHOULD continue with its request. This interim response is used to
 * inform the client that the initial part of the request has been received and
 * has not yet been rejected by the server. The client SHOULD continue by sending
 * the remainder of the request or, if the request has already been completed,
 * ignore this response. The server MUST send a final response after the request
 * has been completed. See RFC 2616 section 8.2.3 for detailed discussion of the
 * use and handling of this status code.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
#define HTTP_STATUS_100_REASON_PHRASE	"Continue"

/**
 *
 * The server understands and is willing to comply with the client's request,
 * via the Upgrade message header field (RFC 2616 section 14.42), for a change
 * in the application protocol being used on this connection. The server will
 * switch protocols to those defined by the response's Upgrade header field
 * immediately after the empty line which terminates the 101 response.
 *
 * The protocol SHOULD be switched only when it is advantageous to do so. For
 * example, switching to a newer version of HTTP is advantageous over older
 * versions, and switching to a real-time, synchronous protocol might be
 * advantageous when delivering resources that use such features.
 *
 */
#define HTTP_STATUS_101_REASON_PHRASE	"Switching Protocols"

/**
 *
 * The request has succeeded. The information returned with the response is
 * dependent on the method used in the request, for example:
 *
 * GET    an entity corresponding to the requested resource is sent in
 *        the response;
 *
 * HEAD   the entity-header fields corresponding to the requested
 *        resource are sent in the response without any message-body;
 *
 * POST   an entity describing or containing the result of the action;
 *
 * TRACE  an entity containing the request message as received by the
 *        end server.
 *
 */
#define HTTP_STATUS_200_REASON_PHRASE	"OK"

/**
 *
 * The request has been fulfilled and resulted in a new resource being created.
 * The newly created resource can be referenced by the URI(s) returned in the
 * entity of the response, with the most specific URI for the resource given by
 * a Location header field. The response SHOULD include an entity containing a
 * list of resource characteristics and location(s) from which the user or user
 * agent can choose the one most appropriate. The entity format is specified by
 * the media type given in the Content-Type header field. The origin server MUST
 * create the resource before returning the 201 status code. If the action cannot
 * be carried out immediately, the server SHOULD respond with 202 (Accepted)
 * response instead.
 *
 * A 201 response MAY contain an ETag response header field indicating the
 * current value of the entity tag for the requested variant just created, see
 * RFC 2616 section 14.19.
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 *
 */
#define HTTP_STATUS_201_REASON_PHRASE	"Created"

/**
 *
 * The request has been accepted for processing, but the processing has not been
 * completed.  The request might or might not eventually be acted upon, as it
 * might be disallowed when processing actually takes place. There is no
 * facility for re-sending a status code from an asynchronous operation such as
 * this.
 *
 * The 202 response is intentionally non-committal. Its purpose is to allow a
 * server to accept a request for some other process (perhaps a batch-oriented
 * process that is only run once per day) without requiring that the user
 * agent's connection to the server persist until the process is completed. The
 * entity returned with this response SHOULD include an indication of the
 * request's current status and either a pointer to a status monitor or some
 * estimate of when the user can expect the request to be fulfilled.
 *
 */
#define HTTP_STATUS_202_REASON_PHRASE	"Accepted"

/**
 *
 * The returned metainformation in the entity-header is not the definitive set
 * as available from the origin server, but is gathered from a local or a
 * third-party copy. The set presented MAY be a subset or superset of the
 * original version. For example, including local annotation information about
 * the resource might result in a superset of the metainformation known by the
 * origin server. Use of this response code is not required and is only
 * appropriate when the response would otherwise be 200 (OK).
 *
 */
#define HTTP_STATUS_203_REASON_PHRASE	"Non-Authoritative Information"

/**
 *
 * The server has fulfilled the request but does not need to return an
 * entity-body, and might want to return updated metainformation. The response
 * MAY include new or updated metainformation in the form of entity-headers,
 * which if present SHOULD be associated with the requested variant.
 *
 * If the client is a user agent, it SHOULD NOT change its document view from
 * that which caused the request to be sent. This response is primarily intended
 * to allow input for actions to take place without causing a change to the user
 * agent's active document view, although any new or updated metainformation
 * SHOULD be applied to the document currently in the user agent's active view.
 *
 * The 204 response MUST NOT include a message-body, and thus is always
 * terminated by the first empty line after the header fields.
 *
 */
#define HTTP_STATUS_204_REASON_PHRASE	"No Content"

/**
 *
 * The server has fulfilled the request and the user agent SHOULD reset the
 * document view which caused the request to be sent. This response is primarily
 * intended to allow input for actions to take place via user input, followed by
 * a clearing of the form in which the input is given so that the user can
 * easily initiate another input action. The response MUST NOT include an
 * entity.
 *
 */
#define HTTP_STATUS_205_REASON_PHRASE	"Reset Content"

/**
 *
 * The server has fulfilled the partial GET request for the resource. The request
 * MUST have included a Range header field (RFC 2616 section 14.35) indicating
 * the desired range, and MAY have included an If-Range header field (RFC 2616 
 * section 14.27) to make the request conditional.
 *
 * The response MUST include the following header fields:
 *
 * - Either a Content-Range header field (section 14.16) indicating the range
 *   included with this response, or a multipart/byteranges Content-Type
 *   including Content-Range fields for each part. If a Content-Length header
 *   field is present in the response, its value MUST match the actual number of
 *   OCTETs transmitted in the message-body.
 * - Date
 * - ETag and/or Content-Location, if the header would have been sent in a 200
 *   response to the same request
 * - Expires, Cache-Control, and/or Vary, if the field-value might differ from
 *   that sent in any previous response for the same variant.
 *
 * If the 206 response is the result of an If-Range request that used a strong
 * cache validator (see RFC 2616 section 13.3.3), the response SHOULD NOT include
 * other entity-headers. If the response is the result of an If-Range request
 * that used a weak validator, the response MUST NOT include other
 * entity-headers; this prevents inconsistencies between cached entity-bodies and
 * updated headers. Otherwise, the response MUST include all of the
 * entity-headers that would have been returned with a 200 (OK) response to the
 * same request.
 *
 * A cache MUST NOT combine a 206 response with other previously cached content
 * if the ETag or Last-Modified headers do not match exactly, see RFC 2616
 * section 13.5.4.
 *
 * A cache that does not support the Range and Content-Range headers MUST NOT
 * cache 206 (Partial) responses.
 *
 */
#define HTTP_STATUS_206_REASON_PHRASE	"Partial Content"

/**
 *
 * The requested resource corresponds to any one of a set of representations,
 * each with its own specific location, and agent-driven negotiation information
 * (RFC 2616 section 12) is being provided so that the user (or user agent) can
 * select a preferred representation and redirect its request to that location.
 *
 * Unless it was a HEAD request, the response SHOULD include an entity containing
 * a list of resource characteristics and location(s) from which the user or user
 * agent can choose the one most appropriate. The entity format is specified by
 * the media type given in the Content-Type header field. Depending upon the
 * format and the capabilities of the user agent, selection of the most
 * appropriate choice MAY be performed automatically. However, this
 * specification does not define any standard for such automatic selection.
 *
 * If the server has a preferred choice of representation, it SHOULD include the
 * specific URI for that representation in the Location field; user agents MAY
 * use the Location field value for automatic redirection. This response is
 * cacheable unless indicated otherwise.
 *
 */
#define HTTP_STATUS_300_REASON_PHRASE	"Multiple Choices"

/**
 *
 * The requested resource has been assigned a new permanent URI and any future
 * references to this resource SHOULD use one of the returned URIs.  Clients
 * with link editing capabilities ought to automatically re-link references to
 * the Request-URI to one or more of the new references returned by the server,
 * where possible. This response is cacheable unless indicated otherwise.
 *
 * The new permanent URI SHOULD be given by the Location field in the response.
 * Unless the request method was HEAD, the entity of the response SHOULD contain
 * a short hypertext note with a hyperlink to the new URI(s).
 *
 * If the 301 status code is received in response to a request other than GET or
 * HEAD, the user agent MUST NOT automatically redirect the request unless it can
 * be confirmed by the user, since this might change the conditions under which
 * the request was issued.
 *
 */
#define HTTP_STATUS_301_REASON_PHRASE	"Moved Permanently"

/**
 *
 * The requested resource resides temporarily under a different URI. Since the
 * redirection might be altered on occasion, the client SHOULD continue to use
 * the Request-URI for future requests.  This response is only cacheable if
 * indicated by a Cache-Control or Expires header field.
 *
 * The temporary URI SHOULD be given by the Location field in the response.
 * Unless the request method was HEAD, the entity of the response SHOULD contain
 * a short hypertext note with a hyperlink to the new URI(s).
 *
 * If the 302 status code is received in response to a request other than GET or
 * HEAD, the user agent MUST NOT automatically redirect the request unless it can
 * be confirmed by the user, since this might change the conditions under which
 * the request was issued.
 *
 * Note: RFC 1945 and RFC 2068 specify that the client is not allowed to change
 * the method on the redirected request. However, most existing user agent
 * implementations treat 302 as if it were a 303 response, performing a GET on
 * the Location field-value regardless of the original request method. The
 * status codes 303 and 307 have been added for servers that wish to make
 * unambiguously clear which kind of reaction is expected of the client.
 *
 */
#define HTTP_STATUS_302_REASON_PHRASE	"Found"

/**
 *
 * The response to the request can be found under a different URI and SHOULD be
 * retrieved using a GET method on that resource. This method exists primarily to
 * allow the output of a POST-activated script to redirect the user agent to a
 * selected resource. The new URI is not a substitute reference for the
 * originally requested resource. The 303 response MUST NOT be cached, but the
 * response to the second (redirected) request might be cacheable.
 *
 * The different URI SHOULD be given by the Location field in the response.
 * Unless the request method was HEAD, the entity of the response SHOULD contain
 * a short hypertext note with a hyperlink to the new URI(s).
 *
 * Note: Many pre-HTTP/1.1 user agents do not understand the 303 status. When
 * interoperability with such clients is a concern, the 302 status code may be
 * used instead, since most user agents react to a 302 response as described
 * here for 303.
 *
 */
#define HTTP_STATUS_303_REASON_PHRASE	"See Other"

/**
 *
 * If the client has performed a conditional GET request and access is allowed,
 * but the document has not been modified, the server SHOULD respond with this
 * status code. The 304 response MUST NOT contain a message-body, and thus is
 * always terminated by the first empty line after the header fields.
 *
 * The response MUST include the following header fields:
 *
 * - Date, unless its omission is required by RFC 2616 section 14.18.1
 *
 * If a clockless origin server obeys these rules, and proxies and clients add
 * their own Date to any response received without one (as already specified by
 * [RFC 2068], section 14.19), caches will operate correctly.
 *
 * - ETag and/or Content-Location, if the header would have been sent in a 200
 *   response to the same request
 * - Expires, Cache-Control, and/or Vary, if the field-value might differ from
 *   that sent in any previous response for the same variant
 *
 * If the conditional GET used a strong cache validator (see RFC 2616 section
 * 13.3.3), the response SHOULD NOT include other entity-headers. Otherwise
 * (i.e., the conditional GET used a weak validator), the response MUST NOT
 * include other entity-headers; this prevents inconsistencies between cached
 * entity-bodies and updated headers.
 *
 * If a 304 response indicates an entity not currently cached, then the cache
 * MUST disregard the response and repeat the request without the conditional.
 *
 * If a cache uses a received 304 response to update a cache entry, the cache
 * MUST update the entry to reflect any new field values given in the response.
 *
 */
#define HTTP_STATUS_304_REASON_PHRASE	"Not Modified"

/**
 *
 * The requested resource MUST be accessed through the proxy given by the
 * Location field. The Location field gives the URI of the proxy. The recipient
 * is expected to repeat this single request via the proxy. 305 responses MUST
 * only be generated by origin servers.
 *
 * Note: RFC 2068 was not clear that 305 was intended to redirect a single
 * request, and to be generated by origin servers only.  Not observing these
 * limitations has significant security consequences.
 *
 */
#define HTTP_STATUS_305_REASON_PHRASE	"Use Proxy"

/**
 *
 * The requested resource resides temporarily under a different URI. Since the
 * redirection MAY be altered on occasion, the client SHOULD continue to use the
 * Request-URI for future requests.  This response is only cacheable if indicated
 * by a Cache-Control or Expires header field.
 *
 * The temporary URI SHOULD be given by the Location field in the response.
 * Unless the request method was HEAD, the entity of the response SHOULD contain
 * a short hypertext note with a hyperlink to the new URI(s) , since many
 * pre-HTTP/1.1 user agents do not understand the 307 status. Therefore, the
 * note SHOULD contain the information necessary for a user to repeat the
 * original request on the new URI.
 *
 * If the 307 status code is received in response to a request other than GET or
 * HEAD, the user agent MUST NOT automatically redirect the request unless it can
 * be confirmed by the user, since this might change the conditions under which
 * the request was issued.
 *
 */
#define HTTP_STATUS_307_REASON_PHRASE	"Temporary Redirect"

/**
 *
 * The request could not be understood by the server due to malformed syntax.
 * The client SHOULD NOT repeat the request without modifications.
 *
 */
#define HTTP_STATUS_400_REASON_PHRASE	"Bad Request"

/**
 *
 * The request requires user authentication. The response MUST include a
 * WWW-Authenticate header field (RFC 2616 section 14.47) containing a challenge
 * applicable to the requested resource. The client MAY repeat the request with
 * a suitable Authorization header field (RFC 2616 section 14.8). If the request
 * already included Authorization credentials, then the 401 response indicates
 * that authorization has been refused for those credentials. If the 401
 * response contains the same challenge as the prior response, and the user
 * agent has already attempted authentication at least once, then the user
 * SHOULD be presented the entity that was given in the response, since that
 * entity might include relevant diagnostic information. HTTP access
 * authentication is explained in RFC 2617 "HTTP Authentication: Basic and
 * Digest Access Authentication"
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 * @see http://www.ietf.org/rfc/rfc2617.txt
 *
 */
#define HTTP_STATUS_401_REASON_PHRASE	"Unauthorized"

/**
 *
 * This code is reserved for future use.
 *
 */
#define HTTP_STATUS_402_REASON_PHRASE	"Payment Required"

/**
 *
 * The server understood the request, but is refusing to fulfill it.
 * Authorization will not help and the request SHOULD NOT be repeated. If the
 * request method was not HEAD and the server wishes to make public why the
 * request has not been fulfilled, it SHOULD describe the reason for the refusal
 * in the entity.  If the server does not wish to make this information
 * available to the client, the status code 404 (Not Found) can be used instead.
 *
 */
#define HTTP_STATUS_403_REASON_PHRASE	"Forbidden"

/**
 *
 * The server has not found anything matching the Request-URI. No indication is
 * given of whether the condition is temporary or permanent. The 410 (Gone)
 * status code SHOULD be used if the server knows, through some internally
 * configurable mechanism, that an old resource is permanently unavailable and
 * has no forwarding address. This status code is commonly used when the server
 * does not wish to reveal exactly why the request has been refused, or when no
 * other response is applicable.
 *
 */
#define HTTP_STATUS_404_REASON_PHRASE	"Not Found"

/**
 *
 * The method specified in the Request-Line is not allowed for the resource
 * identified by the Request-URI. The response MUST include an Allow header
 * containing a list of valid methods for the requested resource.
 *
 */
#define HTTP_STATUS_405_REASON_PHRASE	"Method Not Allowed"

/**
 *
 * The resource identified by the request is only capable of generating response
 * entities which have content characteristics not acceptable according to the
 * accept headers sent in the request.
 *
 * Unless it was a HEAD request, the response SHOULD include an entity containing
 * a list of available entity characteristics and location(s) from which the user
 * or user agent can choose the one most appropriate. The entity format is
 * specified by the media type given in the Content-Type header field. Depending
 * upon the format and the capabilities of the user agent, selection of the most
 * appropriate choice MAY be performed automatically. However, this specification
 * does not define any standard for such automatic selection.
 *
 * Note: HTTP/1.1 servers are allowed to return responses which are not
 * acceptable according to the accept headers sent in the request. In some cases,
 * this may even be preferable to sending a 406 response. User agents are
 * encouraged to inspect the headers of an incoming response to determine if it
 * is acceptable.
 *
 * If the response could be unacceptable, a user agent SHOULD temporarily stop
 * receipt of more data and query the user for a decision on further actions.
 *
 */
#define HTTP_STATUS_406_REASON_PHRASE	"Not Acceptable"

/**
 *
 * This code is similar to 401 (Unauthorized), but indicates that the client
 * must first authenticate itself with the proxy. The proxy MUST return a
 * Proxy-Authenticate header field (RFC 2616 section 14.33) containing a
 * challenge applicable to the proxy for the requested resource. The client MAY
 * repeat the request with a suitable Proxy-Authorization header field (RFC 2616
 * section 14.34). HTTP access authentication is explained in RFC 2617 "HTTP
 * Authentication: Basic and Digest Access Authentication".
 *
 * @see http://www.ietf.org/rfc/rfc2616.txt
 * @see http://www.ietf.org/rfc/rfc2617.txt
 *
 */
#define HTTP_STATUS_407_REASON_PHRASE	"Proxy Authentication Required"

/**
 *
 * The client did not produce a request within the time that the server was
 * prepared to wait. The client MAY repeat the request without modifications at
 * any later time.
 *
 */
#define HTTP_STATUS_408_REASON_PHRASE	"Request Time-out"

/**
 *
 * The request could not be completed due to a conflict with the current state
 * of the resource. This code is only allowed in situations where it is expected
 * that the user might be able to resolve the conflict and resubmit the request.
 * The response body SHOULD include enough information for the user to recognize
 * the source of the conflict. Ideally, the response entity would include enough
 * information for the user or user agent to fix the problem; however, that
 * might not be possible and is not required.
 *
 * Conflicts are most likely to occur in response to a PUT request. For example,
 * if versioning were being used and the entity being PUT included changes to a
 * resource which conflict with those made by an earlier (third-party) request,
 * the server might use the 409 response to indicate that it can't complete the
 * request. In this case, the response entity would likely contain a list of the
 * differences between the two versions in a format defined by the response
 * Content-Type.
 *
 */
#define HTTP_STATUS_409_REASON_PHRASE	"Conflict"

/**
 *
 * The requested resource is no longer available at the server and no forwarding
 * address is known. This condition is expected to be considered permanent.
 * Clients with link editing capabilities SHOULD delete references to the
 * Request-URI after user approval. If the server does not know, or has no
 * facility to determine, whether or not the condition is permanent, the status
 * code 404 (Not Found) SHOULD be used instead. This response is cacheable unless
 * indicated otherwise.
 *
 * The 410 response is primarily intended to assist the task of web maintenance
 * by notifying the recipient that the resource is intentionally unavailable and
 * that the server owners desire that remote links to that resource be removed.
 * Such an event is common for limited-time, promotional services and for
 * resources belonging to individuals no longer working at the server's site. It
 * is not necessary to mark all permanently unavailable resources as "gone" or to
 * keep the mark for any length of time -- that is left to the discretion of the
 * server owner.
 *
 */
#define HTTP_STATUS_410_REASON_PHRASE	"Gone"

/**
 *
 * The server refuses to accept the request without a defined Content- Length.
 * The client MAY repeat the request if it adds a valid Content-Length header
 * field containing the length of the message-body in the request message.
 *
 */
#define HTTP_STATUS_411_REASON_PHRASE	"Length Required"

/**
 *
 * The precondition given in one or more of the request-header fields evaluated
 * to false when it was tested on the server. This response code allows the
 * client to place preconditions on the current resource metainformation (header
 * field data) and thus prevent the requested method from being applied to a
 * resource other than the one intended.
 *
 */
#define HTTP_STATUS_412_REASON_PHRASE	"Precondition Failed"

/**
 *
 * The server is refusing to process a request because the request entity is
 * larger than the server is willing or able to process. The server MAY close
 * the connection to prevent the client from continuing the request.
 *
 * If the condition is temporary, the server SHOULD include a Retry-After header
 * field to indicate that it is temporary and after what time the client MAY try
 * again.
 *
 */
#define HTTP_STATUS_413_REASON_PHRASE	"Request Entity Too Large"

/**
 *
 * The server is refusing to service the request because the Request-URI is
 * longer than the server is willing to interpret. This rare condition is only
 * likely to occur when a client has improperly converted a POST request to a
 * GET request with long query information, when the client has descended into a
 * URI "black hole" of redirection (e.g., a redirected URI prefix that points to
 * a suffix of itself), or when the server is under attack by a client attempting
 * to exploit security holes present in some servers using fixed-length buffers
 * for reading or manipulating the Request-URI.
 *
 */
#define HTTP_STATUS_414_REASON_PHRASE	"Request-URI Too Large"

/**
 *
 * The server is refusing to service the request because the entity of the
 * request is in a format not supported by the requested resource for the
 * requested method.
 *
 */
#define HTTP_STATUS_415_REASON_PHRASE	"Unsupported Media Type"

/**
 *
 * A server SHOULD return a response with this status code if a request included
 * a Range request-header field (RFC 2616 section 14.35), and none of the
 * range-specifier values in this field overlap the current extent of the
 * selected resource, and the request did not include an If-Range request-header
 * field. (For byte-ranges, this means that the first-byte-pos of all of the
 * byte-range-spec values were greater than the current length of the selected
 * resource.)
 *
 * When this status code is returned for a byte-range request, the response
 * SHOULD include a Content-Range entity-header field specifying the current
 * length of the selected resource (see RFC 2616 section 14.16). This response
 * MUST NOT use the multipart/byteranges content-type.
 *
 */
#define HTTP_STATUS_416_REASON_PHRASE	"Requested range not satisfiable"

/**
 *
 * The expectation given in an Expect request-header field (see RFC 2616 section
 * 14.20) could not be met by this server, or, if the server is a proxy, the
 * server has unambiguous evidence that the request could not be met by the
 * next-hop server.
 *
 */
#define HTTP_STATUS_417_REASON_PHRASE	"Expectation Failed"

/**
 *
 * The server encountered an unexpected condition which prevented it from
 * fulfilling the request.
 *
 */
#define HTTP_STATUS_500_REASON_PHRASE	"Internal Server Error"

/**
 *
 * The server does not support the functionality required to fulfill the request.
 * This is the appropriate response when the server does not recognize the
 * request method and is not capable of supporting it for any resource.
 *
 */
#define HTTP_STATUS_501_REASON_PHRASE	"Not Implemented"

/**
 *
 * The server, while acting as a gateway or proxy, received an invalid response
 * from the upstream server it accessed in attempting to fulfill the request.
 *
 */
#define HTTP_STATUS_502_REASON_PHRASE	"Bad Gateway"

/**
 *
 * The server is currently unable to handle the request due to a temporary
 * overloading or maintenance of the server. The implication is that this is a
 * temporary condition which will be alleviated after some delay. If known, the
 * length of the delay MAY be indicated in a Retry-After header. If no
 * Retry-After is given, the client SHOULD handle the response as it would for a
 * 500 response.
 *
 * Note: The existence of the 503 status code does not imply that a server must
 * use it when becoming overloaded. Some servers may wish to simply refuse the
 * connection.
 *
 */
#define HTTP_STATUS_503_REASON_PHRASE	"Service Unavailable"

/**
 *
 * The server, while acting as a gateway or proxy, did not receive a timely
 * response from the upstream server specified by the URI (e.g. HTTP, FTP, LDAP)
 * or some other auxiliary server (e.g. DNS) it needed to access in attempting
 * to complete the request.
 *
 * Note: Note to implementors: some deployed proxies are known to return 400 or
 * 500 when DNS lookups time out.
 *
 */
#define HTTP_STATUS_504_REASON_PHRASE	"Gateway Time-out"

/**
 *
 * The server does not support, or refuses to support, the HTTP protocol version
 * that was used in the request message. The server is indicating that it is
 * unable or unwilling to complete the request using the same major version as
 * the client, as described in RFC 2616 section 3.1, other than with this error
 * message. The response SHOULD contain an entity describing why that version is
 * not supported and what other protocols are supported by that server.
 *
 */
#define HTTP_STATUS_505_REASON_PHRASE	"HTTP Version not supported"

/*@}*/

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
 * @return A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *         to free the pair.
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
 * @return A newly crated hpair_t object. Use hpair_free() or hpair_free_deep()
 *         to free the pair.
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
 * @return a newly created pair with the same (key,value) pairs as in 'src'.
 *         This fields will be cloned. The'next' field will be set to NULL.
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
extern hpair_t *hpairnode_copy_deep(const hpair_t *src);

/**
 *
 * Dumps a set of pairs.
 *
 * @see hpairnode_dump
 *
 */
extern void hpairnode_dump_deep(const hpair_t *pairs);

/**
 *
 * Dumps the pair specified.
 *
 */
extern void hpairnode_dump(const hpair_t *pair);

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
 *
 */
extern content_type_t *content_type_new(const char *content_type_str);

/**
 *
 * Frees the given content_type_t object
 *
 */
extern void content_type_free(content_type_t * ct);

extern struct part_t *part_new(const char *id, const char *filename, const char *content_type, const char *transfer_encoding, struct part_t * next);

extern void part_free(struct part_t * part);


/* should be used internally */
extern struct attachments_t *attachments_new(void);

/**
 *
 * Free a attachment. Create attachments with MIME
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
