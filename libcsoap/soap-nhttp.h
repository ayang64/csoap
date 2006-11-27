/******************************************************************
*  $Id: soap-nhttp.h,v 1.4 2006/11/27 10:49:57 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2007 Heiko Ronsdorf
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
* Email: hero@persua.de
******************************************************************/
#ifndef __soap_nhttp_h
#define __soap_nhttp_h

/** @file
 *
 * Using SOAP in HTTP
 *
 * This section describes how to use SOAP within HTTP with or without using the
 * HTTP Extension Framework. Binding SOAP to HTTP provides the advantage of being
 * able to use the formalism and decentralized flexibility of SOAP with the rich
 * feature set of HTTP. Carrying SOAP in HTTP does not mean that SOAP overrides
 * existing semantics of HTTP but rather that the semantics of SOAP over HTTP
 * maps naturally to HTTP semantics.
 *
 * SOAP naturally follows the HTTP request/response message model providing SOAP
 * request parameters in a HTTP request and SOAP response parameters in a HTTP
 * response. Note, however, that SOAP intermediaries are NOT the same as HTTP
 * intermediaries. That is, an HTTP intermediary addressed with the HTTP
 * Connection header field cannot be expected to inspect or process the SOAP
 * entity body carried in the HTTP request.
 *
 * HTTP applications MUST use the media type "text/xml" according to RFC 2376
 * when including SOAP entity bodies in HTTP messages.
 *
 * @see http://www.ietf.org/rfc/rfc2376.txt
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * The SOAPAction HTTP request header field can be used to indicate the intent of
 * the SOAP HTTP request. The value is a URI identifying the intent. SOAP places
 * no restrictions on the format or specificity of the URI or that it is
 * resolvable. An HTTP client MUST use this header field when issuing a SOAP
 * HTTP Request. The presence and content of the SOAPAction header field can be
 * used by servers such as firewalls to appropriately filter SOAP request
 * messages in HTTP. The header field value of empty string ("") means that the
 * intent of the SOAP message is provided by the HTTP Request-URI. No value
 * means that there is no indication of the intent of the message.
 *
 */
#define SOAP_NHTTP_SOAP_ACTION	"SoapAction"

extern herror_t soap_nhttp_server_init_args(int argc, char **argv);
extern herror_t soap_nhttp_server_run(void);
extern void soap_nhttp_server_destroy(void);

extern herror_t soap_nhttp_register(const char *context);

extern short soap_nhttp_get_port(void);
extern const char *soap_nhttp_get_protocol(void);

extern herror_t soap_nhttp_client_init_args(int argc, char **argv);
extern herror_t soap_nhttp_client_invoke(void *unused, struct SoapCtx *req, struct SoapCtx **res);
extern void soap_nhttp_client_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
