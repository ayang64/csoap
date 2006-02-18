/******************************************************************
 *  $Id: nanohttp-socket.h,v 1.22 2006/02/18 20:14:36 snowdrop Exp $
 *
 * CSOAP Project:  A http client/server library in C
 * Copyright (C) 2003  Ferhat Ayaz
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef NANO_HTTP_SOCKET_H
#define NANO_HTTP_SOCKET_H

#include <nanohttp/nanohttp-common.h>

#include <time.h>

#ifdef HAVE_SSL
#include <openssl/ssl.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#endif


/*
  Socket definition
*/
typedef struct hsocket_t
{

#ifdef HAVE_SSL
  SSL *ssl;
  SSL_CTX *sslCtx;
#endif

#ifdef WIN32
  SOCKET sock;
#else
  int sock;
#endif
  int block;

} hsocket_t; /* end of socket definition */

#ifdef WIN32
typedef int socklen_t;
#endif




/**
  Initializes the socket modul. This should be called only 
  once for an application.

  @returns This function should always return H_OK. 
 */
herror_t hsocket_module_init();


/**
  Destroys the socket modul. This should be called after 
  finishing an application.
*/
void hsocket_module_destroy();


/**
  Initializes a given socket object. This function (or 
  hsokcet_init_ssl) should
  be called for every socket before using it.

  @param sock the destination socket to initialize.

  @see hsocket_init_ssl
  @returns This function should always return H_OK. 
 */
herror_t hsocket_init(hsocket_t * sock);


/**
   Initializes a given socket object with ssl context.
   To initialize the socket without ssl, you should use
   hsocket_init()

   @param sock  the destination socket to initialize.
   @param sslCert keyfile
   @param sslPass passwort
   @param sslCA calist

   @see hsocket_init
   @returns HSOCKET_ERROR_SSLCTX if failed. H_OK otherwise
 */
herror_t
hsocket_init_ssl(hsocket_t * sock, 
		 const char* sslCert, 
		 const char* sslPass,
		 const char* sslCA);

/**
  Destroys and releases a given socket.

  @param sock the socket to destroy
*/
void hsocket_free(hsocket_t sock);


/**
  Connects to a given host. The hostname can be an IP number 
  or a humen readable hostname.
  
  @param sock the destonation socket object to use
  @param host hostname 
  @param port port number to connect to

  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_CREATE 
    <BR>HSOCKET_ERROR_GET_HOSTNAME 
    <BR>HSOCKET_ERROR_CONNECT
 */
herror_t hsocket_open(hsocket_t * sock, const char *host, int port);


/**
  Close a socket connection.

  @param sock the socket to close
*/
void hsocket_close(hsocket_t sock);


/**
  Binds a socket to a given port number. After bind you
  can call hsocket_listen() to listen to the port.

  @param sock socket to use.
  @param port  port number to bind to
  
  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_CREATE
    <BR>HSOCKET_ERROR_BIND

  @see hsocket_listen
 */
herror_t hsocket_bind(hsocket_t * sock, int port);


/**
  Set the socket to the listen mode. You must bind 
  the socket to a port with hsocket_bind() before 
  you can listen to the port.

  @param sock the socket to use

  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_NOT_INITIALIZED
    <BR>HSOCKET_ERROR_LISTEN
*/
herror_t hsocket_listen(hsocket_t sock);


/**
  Accepts an incoming socket request. Note that this function
  will not return until a socket connection is ready.

  @param sock the socket which listents to a port
  @param dest the destination socket which will be created
  
  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_NOT_INITIALIZED
    <BR>HSOCKET_ERROR_ACCEPT
*/
herror_t hsocket_accept(hsocket_t sock, hsocket_t * dest);


/**
  Sends data throught the socket.

  @param sock the socket to use to send the data
  @param bytes bytes to send
  @param size size of memory to sent pointed by bytes.

  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_NOT_INITIALIZED
    <BR>HSOCKET_ERROR_SEND
*/
herror_t hsocket_nsend(hsocket_t sock, const byte_t * bytes, int size);


/**
  Sends a string throught the socket

  @param sock the socket to use to send the data
  @param str the null terminated string to sent

  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_NOT_INITIALIZED
    <BR>HSOCKET_ERROR_SEND
*/
herror_t hsocket_send(hsocket_t sock, const char *str);


/**
  Reads data from the socket.

  @param sock the socket to read data from
  @param buffer the buffer to use to save the readed bytes
  @param size the maximum size of bytes to read
  @param force if force is 1 then hsocket_read() will wait until
   maximum size of bytes (size parameter) was readed. Otherwise
   this function will not wait and will return with the bytes
   quequed on the socket.

   @returns This function will return -1 if an read error was occured.
     Otherwise the return value is the size of bytes readed from 
     the socket.
  
*/
herror_t hsocket_read(hsocket_t sock, byte_t * buffer, int size, int force,
                      int *readed);

/**
  Sets the goven socket to non-blocking socket mode.

  @param sock the socket to set into the non-blocking mode

  @returns H_OK if success. One of the followings if fails:<P>
    <BR>HSOCKET_ERROR_NOT_INITIALIZED
    <BR>HSOCKET_ERROR_IOCTL
*/
herror_t hsocket_block(hsocket_t sock, int block);


#endif
