/******************************************************************
 *  $Id: csoapsocket.c,v 1.1 2003/03/25 22:17:26 snowdrop Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
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
 * Email: ayaz@jprogrammet.net
 ******************************************************************/
#include "csoapsocket.h"
#include "csoaperror.h"
#include "csoaplog.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SOCKET_MAX_BUFSIZE 1054


/*-----------------------------------------------------------------
  FUNCTION: SoapSocketCreate
/-----------------------------------------------------------------*/

CSOAP_STATUS SoapSocketCreate(const char* hostname, int port, HSOAPSOCKET* hSock)
{

  int sock;
  struct sockaddr_in address; 
  struct hostent* host;
  int i;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock <= 0) return ERROR_SOCKET_CAN_NOT_CREATE_SOCKET;

  /* Get host data */
  host = gethostbyname(hostname);
  if (host == NULL) return ERROR_SOCKET_CAN_NOT_GET_HOSTNAME;

  /* set server addresss */
  address.sin_family = host->h_addrtype;;
  address.sin_port = htons(port);
  memcpy((char*)&address.sin_addr.s_addr,
	 host->h_addr_list[0], host->h_length);

  /* connect to the server */
  if (connect(sock, (struct sockaddr*) &address, sizeof(address)) != 0)
    return ERROR_SOCKET_CAN_NOT_CONNECT;
	
  *hSock = sock;
  return CSOAP_OK;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapSocketRecv
/-----------------------------------------------------------------*/

CSOAP_STATUS SoapSocketRecv(HSOAPSOCKET sock, char** buffer, ssize_t* totalSize)
{
  const char *FUNC = "SoapSocketRecv";
  ssize_t size;
  int chunk=1;
  char tmp[SOCKET_MAX_BUFSIZE+1];

  SoapTraceEnter( FUNC, "Entering");
  *totalSize = 0;
  *buffer = 0;

  do {
    SoapLog(LOG_DEBUG, FUNC, "Receiving buffer\n");
    size = recv(sock, tmp, SOCKET_MAX_BUFSIZE, 0);
    SoapLog(LOG_DEBUG, FUNC, "received = %s\n", tmp);
    if (size == -1) {
      printf("Error reading from socket\n");
      return ERROR_SOCKET_CAN_NOT_RECEIVE;
    }
    *totalSize += size;
    if (*buffer) {
      SoapLog(LOG_DEBUG, FUNC, "realloc\n");
      *buffer = (char*)realloc(*buffer, SOCKET_MAX_BUFSIZE*chunk+1);
      strcat(*buffer, tmp);
      SoapLog(LOG_DEBUG, FUNC,"realloc ok\n");
    } else {
      SoapLog(LOG_DEBUG, FUNC, "alloc\n");
      *buffer = (char*)realloc(NULL, SOCKET_MAX_BUFSIZE+1);
      strcpy(*buffer, tmp);
      SoapLog(LOG_DEBUG, FUNC,"alloc ok\n");
    }

    SoapLog(LOG_DEBUG, FUNC, "totalSize = %d\n", *totalSize);
    (*buffer)[*totalSize] = '\0';	
    chunk++;
  } while (size > 0);

  SoapTraceLeave(FUNC, "totalsize: %d", *totalSize);
  return CSOAP_OK;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapSocketSend
/-----------------------------------------------------------------*/

CSOAP_STATUS SoapSocketSend(HSOAPSOCKET sock, const char* req, ssize_t* sentSize)
{
  const char *FUNC = "SoapSocketSend";
  ssize_t size;

  SoapTraceEnter(FUNC, "");
  
  size = send(sock, req, strlen(req), 0);
  if (size == -1)
    return ERROR_SOCKET_CAN_NOT_SEND;

  *sentSize = size;
  SoapTraceLeave(FUNC, "");
  return CSOAP_OK;
}


/*-----------------------------------------------------------------
  FUNCTION: SoapSocketInitialize
/-----------------------------------------------------------------*/

CSOAP_STATUS SoapSocketInitialize()
{                         
  const char *FUNC = "SoapSocketInitialize";
  SoapTraceEnter(FUNC, "");

#ifdef WIN32  
  /* Initialize WinSocket for the Windows version */
#endif

  SoapTraceLeave(FUNC, "");
  return CSOAP_OK;
}


