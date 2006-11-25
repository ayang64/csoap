/******************************************************************
 *  $Id: nanohttp-error.h,v 1.2 2006/11/25 15:38:10 m0gg Exp $
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
#define HSOCKET_ERROR			1000
#define HSOCKET_ERROR_CREATE		(HSOCKET_ERROR + 1)
#define HSOCKET_ERROR_GET_HOSTNAME	(HSOCKET_ERROR + 2)
#define HSOCKET_ERROR_CONNECT		(HSOCKET_ERROR + 3)
#define HSOCKET_ERROR_SEND		(HSOCKET_ERROR + 4)
#define HSOCKET_ERROR_RECEIVE		(HSOCKET_ERROR + 5)
#define HSOCKET_ERROR_BIND		(HSOCKET_ERROR + 6)
#define HSOCKET_ERROR_LISTEN		(HSOCKET_ERROR + 7)
#define HSOCKET_ERROR_ACCEPT		(HSOCKET_ERROR + 8)
#define HSOCKET_ERROR_NOT_INITIALIZED	(HSOCKET_ERROR + 9)
#define HSOCKET_ERROR_IOCTL		(HSOCKET_ERROR + 10)
#define HSOCKET_ERROR_SSLCLOSE		(HSOCKET_ERROR + 11)
#define HSOCKET_ERROR_SSLCTX		(HSOCKET_ERROR + 11)

/* URL errors */
#define URL_ERROR			1100
#define URL_ERROR_UNKNOWN_PROTOCOL	(URL_ERROR + 1)
#define URL_ERROR_NO_PROTOCOL		(URL_ERROR + 2)
#define URL_ERROR_NO_HOST		(URL_ERROR + 3)

/* Stream errors */
#define STREAM_ERROR			1200
#define STREAM_ERROR_INVALID_TYPE	(STREAM_ERROR + 1)
#define STREAM_ERROR_SOCKET_ERROR	(STREAM_ERROR + 2)
#define STREAM_ERROR_NO_CHUNK_SIZE	(STREAM_ERROR + 3)
#define STREAM_ERROR_WRONG_CHUNK_SIZE	(STREAM_ERROR + 4)

/* MIME errors */
#define MIME_ERROR			1300
#define MIME_ERROR_NO_BOUNDARY_PARAM	(MIME_ERROR + 1)
#define MIME_ERROR_NO_START_PARAM	(MIME_ERROR + 2)
#define MIME_ERROR_PARSE_ERROR		(MIME_ERROR + 3)
#define MIME_ERROR_NO_ROOT_PART		(MIME_ERROR + 4)
#define MIME_ERROR_NOT_MIME_MESSAGE	(MIME_ERROR + 5)

/* General errors */
#define GENERAL_ERROR			1400
#define GENERAL_INVALID_PARAM		(GENERAL_ERROR + 1)
#define GENERAL_HEADER_PARSE_ERROR	(GENERAL_ERROR + 2)

/* Thread errors */
#define THREAD_ERROR			1500
#define THREAD_BEGIN_ERROR		(THREAD_ERROR)

/* XML Errors */
#define XML_ERROR			1600
#define XML_ERROR_EMPTY_DOCUMENT	(XML_ERROR + 1)
#define XML_ERROR_PARSE			(XML_ERROR + 2)

/* SSL Errors */
#define HSSL_ERROR			1700
#define HSSL_ERROR_CA_LIST		(HSSL_ERROR + 10)
#define HSSL_ERROR_CONTEXT		(HSSL_ERROR + 20)
#define HSSL_ERROR_CERTIFICATE		(HSSL_ERROR + 30)
#define HSSL_ERROR_PEM			(HSSL_ERROR + 40)
#define HSSL_ERROR_CLIENT		(HSSL_ERROR + 50)
#define HSSL_ERROR_SERVER		(HSSL_ERROR + 60)
#define HSSL_ERROR_CONNECT		(HSSL_ERROR + 70)

/* File errors */
#define FILE_ERROR			8000
#define FILE_ERROR_OPEN			(FILE_ERROR + 1)
#define FILE_ERROR_READ			(FILE_ERROR + 2)

typedef void *herror_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Creates a new error struture.
 *
 * @see printf
 *
 */
extern herror_t herror_new(const char *func, int errcode, const char *format, ...);

/**
 *
 * Returns the code of the error.
 *
 */
extern int herror_code(herror_t err);

/**
 *
 * Returns the name of the function.
 *
 */
extern const char *herror_func(herror_t err);

/**
 *
 * Returns the error message.
 *
 */
extern const char *herror_message(herror_t err);

/**
 *
 * Frees the error structure.
 *
 */
extern void herror_release(herror_t err);

#ifdef __cplusplus
}
#endif

#endif
