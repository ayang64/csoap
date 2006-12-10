/******************************************************************
 *  $Id: nanohttp-url.h,v 1.1 2006/12/08 21:21:41 m0gg Exp $
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
#ifndef __nanohttp_url_h
#define __nanohttp_url_h

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

#define URL_MAX_HOST_SIZE      120
#define URL_MAX_CONTEXT_SIZE  1024

/**
 *
 * The URL object. A representation of an URL like:
 *
 * [protocol]://[host]:[port]/[context]
 *
 * @see http://www.ietf.org/rfc/rfc2396.txt
 *
 */
struct hurl_t
{
  /**
   *
   * The transfer protocol. Note that only PROTOCOL_HTTP and PROTOCOL_HTTPS are
   * supported by nanohttp.
   *
   */
  hprotocol_t protocol;

  /**
   *
   * The port number. If no port number was given in the URL, one of the default
   * port numbers will be selected. 
   * - URL_HTTP_DEFAULT_PORT    
   * - URL_HTTPS_DEFAULT_PORT   
   * - URL_FTP_DEFAULT_PORT    
   *
   */
  short port;

  /**
   *
   * The hostname
   *
   */
  char host[URL_MAX_HOST_SIZE];

  /**
   *
   * The string after the hostname.
   *
   */
  char context[URL_MAX_CONTEXT_SIZE];
};

#ifdef __cplusplus
extern "C" {
#endif

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
extern herror_t hurl_parse(struct hurl_t * obj, const char *url);

/**
 *
 * Frees the resources within a url and the url itself.
 *
 */
extern void hurl_free(struct hurl_t *url);

#ifdef __cplusplus
}
#endif

#endif