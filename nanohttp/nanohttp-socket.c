/******************************************************************
*  $Id: nanohttp-socket.c,v 1.45 2006/01/31 18:33:05 mrcsys Exp $
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
#include <nanohttp/nanohttp-socket.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-ssl.h>

#ifdef WIN32
#include "wsockcompat.h"
#include <winsock2.h>
#include <process.h>

#ifndef __MINGW32__
typedef int ssize_t;
#endif

#else
#include <fcntl.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef MEM_DEBUG
#include <utils/alloc.h>
#endif


#ifdef WIN32
#undef errno
#define errno WSAGetLastError()
#endif


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

#else /* */
  /* nothing to init for unix sockets */
#endif /* */
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

#else /* */
  /* nothing to destroy for unix sockets */
#endif /* */
}

/*--------------------------------------------------
FUNCTION: hsocket_init
----------------------------------------------------*/
herror_t
hsocket_init(hsocket_t * sock)
{
  log_verbose1("Starting hsocket init");
  /* just set the descriptor to -1 */
  sock->sock = -1;

#ifdef HAVE_SSL
  sock->ssl = NULL;
  sock->sslCtx = NULL;
#endif

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

  sock.sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock.sock <= 0)
    return herror_new("hsocket_open", HSOCKET_ERROR_CREATE,
                      "Socket error: %d", errno);

  /* Get host data */
  host = gethostbyname(hostname);
  if (host == NULL)
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
    log_verbose1("Using HTTPS");
    dsock->ssl = init_ssl(dsock->sslCtx, sock.sock, SSL_CLIENT);
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
  sock.sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock.sock == -1)
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

#if 0
static void
_hsocket_wait_until_receive(hsocket_t sock)
{
  fd_set fds;
  struct timeval timeout;


  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  while (1)
  {
    log_verbose1("waiting until receive mode");
    /* zero and set file descriptior */
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    /* select socket descriptor */
    switch (select(sock + 1, &fds, NULL, NULL, &timeout))
    {
    case 0:
      /* descriptor is not ready */
      continue;
    case -1:
      /* got a signal? */
      continue;
    default:
      /* no nothing */
      break;
    }
    if (FD_ISSET(sock, &fds))
    {
      break;
    }
  }
}
#endif

/*--------------------------------------------------
FUNCTION: hsocket_close
----------------------------------------------------*/
void
hsocket_close(hsocket_t sock)
{
  char junk[10];
/*  _hsocket_wait_until_receive(sock);*/
  log_verbose2("closing socket %d...", sock.sock);
/*  
  struct linger _linger;
	hsocket_block(sock,1);
  _linger.l_onoff =1;
  _linger.l_linger = 30000;
  setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&_linger, sizeof(struct linger));
*/


#ifdef WIN32
  /* shutdown(sock,SD_RECEIVE); */


  shutdown(sock.sock, SD_SEND);
  while (recv(sock.sock, junk, sizeof(junk), 0) > 0)
  {
  };
  closesocket(sock.sock);

#else
  /* XXX m. campbell - It seems like the while loop here needs this */
  fcntl(sock.sock, F_SETFL, O_NONBLOCK);
#ifdef HAVE_SSL
  if (sock.ssl)
  {
    log_verbose1("Closing SSL");
    ssl_cleanup(sock.ssl);
    shutdown(sock.sock, SHUT_RDWR);
    while (recv(sock.sock, junk, sizeof(junk), 0) > 0)
    {
    };
    close(sock.sock);
  }
  else
  {
#endif
    shutdown(sock.sock, SHUT_RDWR);
    while (recv(sock.sock, junk, sizeof(junk), 0) > 0)
    {
    };
    close(sock.sock);
#ifdef HAVE_SSL
  }
#endif
#endif
  log_verbose1("socket closed");
}

#if 0
static int
_test_send_to_file(const char *filename, const byte_t * bytes, int n)
{
  int size;
  FILE *f = fopen(filename, "ab");
  if (!f)
    f = fopen(filename, "wb");
  size = fwrite(bytes, 1, n, f);
  fclose(f);
  return size;
}
#endif
/*--------------------------------------------------
FUNCTION: hsocket_send
----------------------------------------------------*/
herror_t
hsocket_nsend(hsocket_t sock, const byte_t * bytes, int n)
{
  int size;
  int total = 0;

  log_verbose1("Starting to send");
#ifdef HAVE_SSL
  if (sock.sock <= 0 && !sock.ssl)
#else
  if (sock.sock <= 0)
#endif
    return herror_new("hsocket_nsend", HSOCKET_ERROR_NOT_INITIALIZED,
                      "Called hsocket_listen() before initializing!");

  /* log_verbose2( "SENDING %s", bytes );*/

  /* TODO (#1#): check return value and send again until n bytes sent */
  while (1)
  {
#ifdef HAVE_SSL
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
      if (WSAGetLastError() == WSAEWOULDBLOCK)
        continue;
      else
        return herror_new("hsocket_nsend", HSOCKET_ERROR_SEND,
                          "Socket error: %d", errno);
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
#ifdef WIN32
#else
      fcntl(sock.sock, F_SETFL, O_NONBLOCK);
#endif
      /* log_verbose1("START READ LOOP");
       do{
       log_verbose2("DEBUG A %d",i); */
      status = SSL_read(sock.ssl, &buffer[totalRead], total - totalRead);
	  log_verbose2("DEBUG SSL_read %d",status);
	  if (SSL_get_error(sock.ssl, status) == SSL_ERROR_ZERO_RETURN)
      {
        log_verbose1("SSL Error");
        return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "SSL Error");
      }
      if (status < 1)
      {
        int ret = select(sock.sock + 1, &fds, NULL, NULL, &timeout);
        log_verbose2("DEBUG select %d",ret);
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
          /* log_verbose1("DEBUG C"); */
          status = SSL_read(sock.ssl, &buffer[totalRead], total - totalRead);
        }
        /* log_verbose3("DEBUG D char: %d status: %d", 
         buffer[totalRead], SSL_get_error(sock.ssl, status));*/
      }
      /* } while( SSL_get_error(sock.ssl, status) == SSL_ERROR_WANT_READ); */
#ifdef WIN32
#else
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
/*
 switch (errno) {
   case EWOULDBLOCK: 
   case EALREADY:
   case EINPROGRESS:
     return true;
   }
*/

#ifdef HAVE_SSL
    if (sock.ssl && status < 1)
    {

      /* XXX I'm not sure this err_syscall is right here... */
      if (SSL_get_shutdown(sock.ssl) == SSL_RECEIVED_SHUTDOWN ||
          SSL_get_error(sock.ssl, status) == SSL_ERROR_SYSCALL)
      {
        *received = NULL;;
        return herror_new("hsocket_read", HSOCKET_ERROR_SSLCLOSE, "SSL Closed");
      }
      log_error2("Read error (%d)", status);
      log_ssl_error(sock.ssl, status);
      return herror_new("hsocket_read", HSOCKET_ERROR_RECEIVE, "SSL Error");
    }
#endif
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
