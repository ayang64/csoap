/******************************************************************
*  $Id: nanohttp-socket.c,v 1.24 2004/10/15 15:10:37 snowdrop Exp $
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
#include <nanohttp/nanohttp-socket.h>
#include <nanohttp/nanohttp-common.h>

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

#ifdef MEM_DEBUG
#include <utils/alloc.h>
#endif

/*--------------------------------------------------
FUNCTION: hsocket_module_init
NOTE: This will be called from httpd_init()
	for server and from httpc_init() for client
----------------------------------------------------*/
hstatus_t
hsocket_module_init ()
{
#ifdef WIN32
  struct WSAData info;
  WSAStartup (MAKEWORD (2, 2), &info);

#else /* */
  /* nothing to init for unix sockets */
#endif /* */
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_module_destroy
----------------------------------------------------*/
void
hsocket_module_destroy ()
{
#ifdef WIN32
  WSACleanup ();

#else /* */
  /* nothing to destroy for unix sockets */
#endif /* */
}

/*--------------------------------------------------
FUNCTION: hsocket_init
----------------------------------------------------*/
hstatus_t
hsocket_init (hsocket_t * sock)
{
  /* just set the descriptor to -1 */
  *sock = -1;
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_free
----------------------------------------------------*/
void
hsocket_free (hsocket_t sock)
{
  /* nothing to free for unix sockets */
}

/*--------------------------------------------------
FUNCTION: hsocket_open
----------------------------------------------------*/
hstatus_t
hsocket_open (hsocket_t * dsock, const char *hostname, int port)
{
  hsocket_t sock;
  char *ip;
  struct sockaddr_in address;
  struct hostent *host;

  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock <= 0)
    return HSOCKET_ERROR_CREATE;

  /* Get host data */
  host = gethostbyname (hostname);
  if (host == NULL)
    return HSOCKET_ERROR_GET_HOSTNAME;

  ip = inet_ntoa (*(struct in_addr *) *host->h_addr_list);
  address.sin_addr.s_addr = inet_addr (ip);

  /* set server addresss */
  address.sin_family = host->h_addrtype;
  address.sin_port = htons (port);

  /* connect to the server */
  if (connect (sock, (struct sockaddr *) &address, sizeof (address)) != 0)
    return HSOCKET_ERROR_CONNECT;

  *dsock = sock;
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_bind
----------------------------------------------------*/
hstatus_t
hsocket_bind (hsocket_t * dsock, int port)
{
  hsocket_t sock;
  struct sockaddr_in addr;

  /* create socket */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    log_error2 ("Can not create socket: '%s'", strerror (errno));
    return HSOCKET_ERROR_CREATE;
  }
  /* bind socket */
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port); /* short, network byte order */
  addr.sin_addr.s_addr = INADDR_ANY;
  memset (&(addr.sin_zero), '\0', 8);   /* zero the rest of the
                                         * struct */

  if (bind (sock, (struct sockaddr *) &addr, sizeof (struct sockaddr)) == -1)
  {
    log_error2 ("Can not bind: '%s'", strerror (errno));
    return HSOCKET_ERROR_BIND;
  }
  *dsock = sock;
  return H_OK;
}

/*----------------------------------------------------------
FUNCTION: hsocket_accept
----------------------------------------------------------*/
hstatus_t
hsocket_accept (hsocket_t sock, hsocket_t * dest)
{
  socklen_t asize;
  hsocket_t sockfd;
  struct sockaddr_in addr;

  if (sock <= 0)
    return  HSOCKET_ERROR_NOT_INITIALIZED;

  asize = sizeof (struct sockaddr_in);
#ifdef WIN32
  while (1)
  {
    sockfd = accept (sock, (struct sockaddr *) &addr, &asize);
    if (sockfd == INVALID_SOCKET)   {
      if (WSAGetLastError () != WSAEWOULDBLOCK)
        return HSOCKET_ERROR_ACCEPT;
    } else  {
      break;
    }
  }
#else 
/* TODO (#1#): why not a loop like in win32? */
  sockfd = accept (sock, (struct sockaddr *) &addr, &asize);
  if (sockfd == -1) {
    return HSOCKET_ERROR_ACCEPT;
  }
#endif 
/* TODO (#1#): Write to access.log file */

  log_verbose3 ("accept new socket (%d) from '%s'", sockfd,
                SAVE_STR (((char *) inet_ntoa (addr.sin_addr))));

  *dest = sockfd;
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_listen
----------------------------------------------------*/
hstatus_t
hsocket_listen (hsocket_t sock)
{
  if (sock <= 0)
    return  HSOCKET_ERROR_NOT_INITIALIZED;

  if (listen (sock, 15) == -1)
  {
    log_error2 ("Can not listen: '%s'", strerror (errno));
    return HSOCKET_ERROR_LISTEN;
  }

  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_close
----------------------------------------------------*/
void
hsocket_close (hsocket_t sock)
{
#ifdef WIN32
  /*shutdown(sock,SD_RECEIVE);*/
  
/*  struct linger _linger;

  hsocket_block(sock,1);
  _linger.l_onoff =1;
  _linger.l_linger = 30000;
  setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&_linger, sizeof(struct linger));

  
  closesocket (sock);
*/
#else
	/*
	struct linger _linger;
	  hsocket_block(sock,1);
	    _linger.l_onoff =1;
	      _linger.l_linger = 30000;
	        setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&_linger, sizeof(struct linger));


  close (sock);*/
#endif
}

/*--------------------------------------------------
FUNCTION: hsocket_send
----------------------------------------------------*/
hstatus_t
hsocket_nsend (hsocket_t sock, const byte_t *bytes, int n)
{
  int size;

  if (sock <= 0)
    return  HSOCKET_ERROR_NOT_INITIALIZED;

  /* TODO (#1#): check return value and send again until n bytes sent */
  
  while (1)
  {
    size = send((int) sock, bytes, n, 0);
#ifdef WIN32
    if (size == INVALID_SOCKET)
        if (WSAGetLastError () == WSAEWOULDBLOCK)
          continue;
        else       
          return HSOCKET_ERROR_SEND;
#else
    if (size == -1)
        return HSOCKET_ERROR_SEND;
#endif
    n -= size;
    if (n<=0) break;
  }
  return H_OK;
}

/*--------------------------------------------------
FUNCTION: hsocket_send
----------------------------------------------------*/
hstatus_t
hsocket_send (hsocket_t sock, const char *str)
{
  return hsocket_nsend (sock, str, strlen (str));
}

/*
  return: -1 is error. read bytes otherwise
*/
int
hsocket_read (hsocket_t sock, byte_t *buffer, int total, int force)
{
  int status;
  int totalRead;
  int wsa_error;
  totalRead = 0;

  do
  {
    status = recv(sock, &buffer[totalRead], total - totalRead, 0);

#ifdef WIN32
    if (status == INVALID_SOCKET) 
    {
        wsa_error = WSAGetLastError();
        switch (wsa_error)
        {
          case WSAEWOULDBLOCK: 
          case WSAEALREADY:
          case WSAEINPROGRESS:        
             continue;
          default:
             log_error2("WSAGetLastError()=%d", wsa_error);
             return -1;
        }
    }

#else
/*
 switch (errno) {
   case EWOULDBLOCK: 
   case EALREADY:
   case EINPROGRESS:
     return true;
   }
*/
    if (status == -1)
        return -1;
#endif
    
    if (!force) {
        _log_str("socket.recv", buffer, status);
        return status;
    }
    
    totalRead += status;

    if (totalRead == total) {
       _log_str("socket.recv", buffer, totalRead);
      return totalRead;
    }
  }
  while (1);
}


hstatus_t
hsocket_block(hsocket_t sock, int block)
{
#ifdef WIN32
  unsigned long iMode;
#endif

  if (sock <= 0)
    return  HSOCKET_ERROR_NOT_INITIALIZED;

#ifdef WIN32
/*#define HSOCKET_BLOCKMODE 0
#define HSOCKET_NONBLOCKMODE 1
*/
  iMode = (block==0)?1:0; /* Non block mode */
  if (ioctlsocket (sock, FIONBIO, (u_long FAR *) & iMode) == INVALID_SOCKET)
  {
    log_error1 ("ioctlsocket error");
    return HSOCKET_ERROR_IOCTL;
  }
#else /* fcntl(sock, F_SETFL, O_NONBLOCK); */
/* TODO (#1#): check for *nix the non blocking sockets */

#endif
  return H_OK;
}


