/******************************************************************
*  $Id: nanohttp-socket.c,v 1.53 2006/02/27 22:26:02 snowdrop Exp $
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef WIN32
#include "wsockcompat.h"
#include <winsock2.h>
#include <process.h>

#ifndef __MINGW32__
typedef int ssize_t;
#endif

#undef errno
#define errno WSAGetLastError()

#endif

#ifdef MEM_DEBUG
#include <utils/alloc.h>
#endif

#include "nanohttp-socket.h"
#include "nanohttp-common.h"
#include "nanohttp-ssl.h"

#ifdef HAVE_SSL
/*SSL_CTX *SSLctx = NULL;
char *SSLCert = NULL;
char *SSLPass = NULL;
char *SSLCA = NULL;
int SSLCertLess = 0;*/
#endif

/*--------------------------------------------------
FUNCTION: hsocket_module_init
NOTE: This will be called from httpd_init()
	for server and from httpc_init() for client
----------------------------------------------------*/
herror_t
hsocket_module_init()
{
#ifdef WIN32
  struct WSAData info;
  WSAStartup(MAKEWORD(2, 2), &info);
#endif

#ifdef HAVE_SSL
  start_ssl();
#endif

  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_module_destroy
----------------------------------------------------*/
void
hsocket_module_destroy()
{
#ifdef WIN32
  WSACleanup();
#endif

  return;
}

/*--------------------------------------------------
FUNCTION: hsocket_init
----------------------------------------------------*/
herror_t
hsocket_init(hsocket_t * sock)
{
  log_verbose1("Starting hsocket init");

  memset(sock, 0, sizeof(hsocket_t));
  sock->sock = HSOCKET_FREE;

  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_init_ssl
----------------------------------------------------*/
#ifdef HAVE_SSL
herror_t
hsocket_init_ssl(hsocket_t * sock, 
		 const char* sslCert, 
		 const char* sslPass,
		 const char* sslCA)
{
  hsocket_init(sock);

  log_verbose1("calling initialize_ctx()");
  sock->sslCtx = initialize_ctx(sslCert, sslPass, sslCA);
  if (sock->sslCtx == NULL)
  {
    return herror_new("hsocket_init_ctx", HSOCKET_ERROR_SSLCTX,
		      "Unable to initialize SSL CTX");
  }

  return H_OK;
}

#endif


/*--------------------------------------------------
FUNCTION: hsocket_free
----------------------------------------------------*/
void
hsocket_free(hsocket_t sock)
{
  /* nothing to free for unix sockets */
  return;
}

/*--------------------------------------------------
FUNCTION: hsocket_open
----------------------------------------------------*/
herror_t
hsocket_open(hsocket_t * dsock, const char *hostname, int port)
{
  hsocket_t sock;
  char *ip;
  struct sockaddr_in address;
  struct hostent *host;

  if ((sock.sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    return herror_new("hsocket_open", HSOCKET_ERROR_CREATE,
                      "Socket error: %d", errno);

  /* Get host data */
  if (!(host = gethostbyname(hostname)))
    return herror_new("hsocket_open", HSOCKET_ERROR_GET_HOSTNAME,
                      "Socket error: %d", errno);

  ip = inet_ntoa(*(struct in_addr *) *host->h_addr_list);
  address.sin_addr.s_addr = inet_addr(ip);

  /* set server addresss */
  address.sin_family = host->h_addrtype;
  address.sin_port = htons((unsigned short) port);

  /* connect to the server */
  if (connect(sock.sock, (struct sockaddr *) &address, sizeof(address)) != 0)
    return herror_new("hsocket_open", HSOCKET_ERROR_CONNECT,
                      "Socket error: %d", errno);

#ifdef HAVE_SSL
  if (!dsock->sslCtx)
  {
#endif
    log_verbose1("Using HTTP");
    dsock->sock = sock.sock;
#ifdef HAVE_SSL
  }
  else
  {
    herror_t status;
    log_verbose1("Using HTTPS");
    dsock->ssl = init_ssl(dsock->sslCtx, sock.sock, SSL_CLIENT);
    if ((status = hsocket_block(*dsock, dsock->block)) != H_OK)
    {
      log_error1("Cannot make socket non-blocking");
      return status;
    }
  }
#endif

  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_bind
----------------------------------------------------*/
herror_t
hsocket_bind(hsocket_t * dsock, int port)
{
  hsocket_t sock;
  struct sockaddr_in addr;
  int opt = 1;

  /* create socket */
  if ((sock.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    log_error3("Can not create socket: '%s'", "Socket error: %d", errno);
    return herror_new("hsocket_bind", HSOCKET_ERROR_CREATE,
                      "Socket error: %d", errno);
  }

  setsockopt(sock.sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  /* bind socket */
  addr.sin_family = AF_INET;
  addr.sin_port = htons((unsigned short) port); /* short, network byte order */
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(addr.sin_zero), '\0', 8);    /* zero the rest of the struct */

  if (bind(sock.sock, (struct sockaddr *) &addr, sizeof(struct sockaddr)) ==
      -1)
  {
    log_error3("Can not bind: '%s'", "Socket error: %d", errno);
    return herror_new("hsocket_bind", HSOCKET_ERROR_BIND, "Socket error: %d",
                      errno);
  }
  dsock->sock = sock.sock;
  return H_OK;
}

/*----------------------------------------------------------
FUNCTION: hsocket_accept
----------------------------------------------------------*/
herror_t
hsocket_accept(hsocket_t sock, hsocket_t * dest)
{
  socklen_t asize;
  hsocket_t sockfd;
  struct sockaddr_in addr;

  if (sock.sock <= 0)
    return herror_new("hsocket_accept", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen() before initializing!");

  asize = sizeof(struct sockaddr_in);
#ifdef WIN32
  while (1)
  {
    sockfd.sock = accept(sock.sock, (struct sockaddr *) &addr, &asize);
    if (sockfd.sock == INVALID_SOCKET)
    {
      if (WSAGetLastError() != WSAEWOULDBLOCK)
        return herror_new("hsocket_accept", HSOCKET_ERROR_ACCEPT,
                          "Socket error: %d", errno);
    }
    else
    {
      break;
    }
  }
#else
/* TODO (#1#): why not a loop like in win32? */
  sockfd.sock = accept(sock.sock, (struct sockaddr *) &addr, &asize);
  if (sockfd.sock == -1)
  {
    return herror_new("hsocket_accept", HSOCKET_ERROR_ACCEPT,
                      "Socket error: %d", errno);
  }
#endif
/* TODO (#1#): Write to access.log file */

  log_verbose3("accept new socket (%d) from '%s'", sockfd.sock,
               SAVE_STR(((char *) inet_ntoa(addr.sin_addr))));

  dest->sock = sockfd.sock;
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_listen
----------------------------------------------------*/
herror_t
hsocket_listen(hsocket_t sock)
{
  if (sock.sock <= 0)
    return herror_new("hsocket_listen", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen() before initializing!");

  if (listen(sock.sock, 15) == -1)
  {
    log_error3("Can not listen: '%s'", "Socket error: %d", errno);
    return herror_new("hsocket_listen", HSOCKET_ERROR_LISTEN,
                      "Socket error: %d", errno);
  }

  return H_OK;
}

#ifdef WIN32
static inline void
_hsocket_sys_close(hsocket_t *sock)
{
  char junk[10];

  /* shutdown(sock,SD_RECEIVE); */

  shutdown(sock->sock, SD_SEND);
  while (recv(sock->sock, junk, sizeof(junk), 0) > 0) ;
    /* nothing */
  closesocket(sock->sock);

  return;
}
#else
static inline void
_hsocket_sys_close(hsocket_t *sock)
{

  shutdown(sock->sock, SHUT_RDWR);

  return;
}
#endif

/*--------------------------------------------------
FUNCTION: hsocket_close
----------------------------------------------------*/
void
hsocket_close(hsocket_t *sock)
{
  log_verbose3("closing socket %p (%d)...", sock, sock->sock);

#ifdef HAVE_SSL
  if (sock->ssl)
  {
    log_verbose1("Closing SSL");
    ssl_cleanup(sock->ssl);
    sock->ssl = NULL;
  }
#endif

  _hsocket_sys_close(sock);

  log_verbose1("socket closed");

  return;
}

/*--------------------------------------------------
FUNCTION: hsocket_send
----------------------------------------------------*/
herror_t
hsocket_nsend(hsocket_t sock, const byte_t * bytes, int n)
{
  int size;
  int total = 0;

  log_verbose2("Starting to send on sock=%p", &sock);
  if (sock.sock <= 0)
    return herror_new("hsocket_nsend", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen() before initializing!");

  /* log_verbose2( "SENDING %s", bytes );*/

  /* TODO (#1#): check return value and send again until n bytes sent */
  while (1)
  {
#ifdef HAVE_SSL
    log_verbose2("ssl = %p", sock.ssl);
    if (sock.ssl)
    {
      size = SSL_write(sock.ssl, bytes + total, n);
    }
    else
    {
#endif
      size = send((int) sock.sock, bytes + total, n, 0);
#ifdef HAVE_SSL
    }
#endif
    log_verbose2("Sent %d", size);
    /* size = _test_send_to_file(filename, bytes, n); */
#ifdef WIN32
    if (size == INVALID_SOCKET)
    {
      if (WSAGetLastError() == WSAEWOULDBLOCK)
      {
        continue;
      }
      else
      {
        return herror_new("hsocket_nsend", HSOCKET_ERROR_SEND,
                          "Socket error: %d", errno);
      }
    }
#else
    if (size == -1)
    {
#ifdef HAVE_SSL
      if (sock.ssl)
      {
        log_error1("Send error");
        log_ssl_error(sock.ssl, size);
      }
#endif
      return herror_new("hsocket_nsend", HSOCKET_ERROR_SEND,
                        "Socket error: %d", errno);
    }
#endif
    n -= size;
    total += size;
    if (n <= 0)
      break;
  }
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_send
----------------------------------------------------*/
herror_t
hsocket_send(hsocket_t sock, const char *str)
{
  return hsocket_nsend(sock, str, strlen(str));
}

/*
  return: -1 is error. read bytes otherwise
*/
herror_t
hsocket_read(hsocket_t sock, byte_t * buffer, int total, int force,
             int *received)
{
  int status;
  int totalRead;
#ifdef WIN32
  int wsa_error = 0;
#endif
  totalRead = 0;
/*
  log_verbose3("Entering hsocket_read(total=%d,force=%d)", total, force);
*/
  do
  {
#ifdef HAVE_SSL
    if (sock.ssl)
    {
      struct timeval timeout;
      /*int i = 0;*/
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(sock.sock, &fds);
      timeout.tv_sec = 10;
      timeout.tv_usec = 0;
#ifndef WIN32
      fcntl(sock.sock, F_SETFL, O_NONBLOCK);
#endif
      status = SSL_read(sock.ssl, &buffer[totalRead], total - totalRead);

      if(ssl_checkFatal( sock.ssl, status )){
          log_verbose1("SSL Error");
          return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "SSL Error");
      }
      if (SSL_get_shutdown(sock.ssl) == SSL_RECEIVED_SHUTDOWN) {
        log_verbose1("SSL shutdown error");
	return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "SSL shutdown error");
      }

      if (status < 1)
      {
        int ret = select(sock.sock + 1, &fds, NULL, NULL, &timeout);
#ifdef WIN32
        if (ret == SOCKET_ERROR)
        {
          wsa_error = WSAGetLastError();
          log_error2("WSAGetLastError()=%d", wsa_error);
          return herror_new("hsocket_read", HSOCKET_ERROR_RECEIVE,
                            "Socket error: %d", errno);

        }
#endif
        if (ret == 0)
        {
          log_verbose1("Socket timeout");
          return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "Timeout");
        }
        else
        {
          status = SSL_read(sock.ssl, &buffer[totalRead], total - totalRead);
          if(ssl_checkFatal( sock.ssl, status )){
              log_verbose1("SSL Error");
              return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "SSL Error");
          }
        }
      }
#ifndef WIN32
      fcntl(sock.sock, F_SETFL, 0);
#endif
    }
    else
    {
#else /* HAVE_SSL */
    {
#endif /* HAVE_SSL */
      status = recv(sock.sock, &buffer[totalRead], total - totalRead, 0);

#ifdef WIN32
      if (status == INVALID_SOCKET)
      {
        wsa_error = WSAGetLastError();
        switch (wsa_error)
        {
        case WSAEWOULDBLOCK:
          /* case WSAEALREADY: case WSAEINPROGRESS: */
          continue;
        default:
          log_error2("WSAGetLastError()=%d", wsa_error);
          return herror_new("hsocket_read", HSOCKET_ERROR_RECEIVE,
                            "Socket error: %d", errno);
        }
      }
    }
#else
    }

    if (status == -1)
      return herror_new("hsocket_read", HSOCKET_ERROR_RECEIVE,
                        "Socket error: %d", errno);
#endif

    if (!force)
    {
      *received = status;
      /* 
         log_verbose3("Leaving !force (received=%d)(status=%d)", *received,
         status); */
      return H_OK;
    }

    totalRead += status;

    if (totalRead == total)
    {
      *received = totalRead;
      /* 
         log_verbose4("Leaving totalRead == total
         (received=%d)(status=%d)(totalRead=%d)", *received, status,
         totalRead); */
      return H_OK;
    }
  }
  while (1);
}


herror_t
hsocket_block(hsocket_t sock, int block)
{
#ifdef WIN32
  unsigned long iMode;
#endif

  if (sock.sock <= 0)
    return herror_new("hsocket_block", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen() before initializing!");

#ifdef WIN32
/*#define HSOCKET_BLOCKMODE 0
#define HSOCKET_NONBLOCKMODE 1
*/

  iMode = (block == 0) ? 1 : 0; /* Non block mode */
  if (ioctlsocket(sock.sock, FIONBIO, (u_long FAR *) & iMode) ==
      INVALID_SOCKET)
  {
    int err = WSAGetLastError();
    log_error2("ioctlsocket error %d", err);
    return herror_new("hsocket_block", HSOCKET_ERROR_IOCTL,
                      "Socket error: %d", err);
  }
#else /* fcntl(sock, F_SETFL, O_NONBLOCK); */
/* TODO (#1#): check for *nix the non blocking sockets */

#endif
  return H_OK;
}
