/******************************************************************
 *  $Id: nanohttp-error.h,v 1.1 2006/11/25 15:06:58 m0gg Exp $
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
#ifndef __nanohttp_error_h
#define __nanohttp_error_h

/* Success flag */
#define H_OK				0

/* Socket errors */
#define HSOCKET_ERROR_CREATE		1001
#define HSOCKET_ERROR_GET_HOSTNAME	1002
#define HSOCKET_ERROR_CONNECT		1003
#define HSOCKET_ERROR_SEND		1004
#define HSOCKET_ERROR_RECEIVE		1005
#define HSOCKET_ERROR_BIND		1006
#define HSOCKET_ERROR_LISTEN		1007
#define HSOCKET_ERROR_ACCEPT		1008
#define HSOCKET_ERROR_NOT_INITIALIZED	1009
#define HSOCKET_ERROR_IOCTL		1010
#define HSOCKET_ERROR_SSLCLOSE		1011
#define HSOCKET_ERROR_SSLCTX		1011

/* URL errors */
#define URL_ERROR_UNKNOWN_PROTOCOL	1101
#define URL_ERROR_NO_PROTOCOL		1102
#define URL_ERROR_NO_HOST		1103

/* Stream errors */
#define STREAM_ERROR_INVALID_TYPE	1201
#define STREAM_ERROR_SOCKET_ERROR	1202
#define STREAM_ERROR_NO_CHUNK_SIZE	1203
#define STREAM_ERROR_WRONG_CHUNK_SIZE	1204

/* MIME errors */
#define MIME_ERROR_NO_BOUNDARY_PARAM	1301
#define MIME_ERROR_NO_START_PARAM	1302
#define MIME_ERROR_PARSE_ERROR		1303
#define MIME_ERROR_NO_ROOT_PART		1304
#define MIME_ERROR_NOT_MIME_MESSAGE	1305

/* General errors */
#define GENERAL_INVALID_PARAM		1400
#define GENERAL_HEADER_PARSE_ERROR	1401

/* Thread errors */
#define THREAD_BEGIN_ERROR		1500

/* XML Errors */
#define XML_ERROR_EMPTY_DOCUMENT	1600
#define XML_ERROR_PARSE			1601

/* SSL Errors */
#define HSSL_ERROR_CA_LIST		1710
#define HSSL_ERROR_CONTEXT		1720
#define HSSL_ERROR_CERTIFICATE		1730
#define HSSL_ERROR_PEM			1740
#define HSSL_ERROR_CLIENT		1750
#define HSSL_ERROR_SERVER		1760
#define HSSL_ERROR_CONNECT		1770

/* File errors */
#define FILE_ERROR_OPEN			8000
#define FILE_ERROR_READ			8001

typedef void *herror_t;

#ifdef __cplusplus
extern "C" {
#endif

extern herror_t herror_new(const char *func, int errcode, const char *format, ...);
extern int herror_code(herror_t err);
extern char *herror_func(herror_t err);
extern char *herror_message(herror_t err);
extern void herror_release(herror_t err);

#ifdef __cplusplus
}
#endif

#endif
