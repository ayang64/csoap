/******************************************************************
 *  $Id: csoapsocket.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef CSOAP_SOCKET_H
#define CSOAP_SOCKET_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif 

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>
#include "libcsoap/csoaperror.h"

typedef int HSOAPSOCKET;

#define SOCKET_MAX_BUFSIZE 1054


/**
 * Initializes the soap socket module.
 * This is called internally. Before any
 * socket operation.
 * 
 * @return CSOAP_OK if it was successfull
 */
int SoapSocketInitialize();


/**
 * Creates a new soap compatible socket object.
 * 
 * @param host the host to connect
 * @param port the port to connect
 * @param socket the destination pointer
 *  to receive a connected socket object
 * 
 * @return CSOAP_OK if it was successfull.
 */
CSOAP_STATUS SoapSocketCreate(const char* host, int port, HSOAPSOCKET* socket);

/**
 * Sends data to the connected socket.
 *
 * @param sock A connected socket object
 * @param req data to send
 * @param sentSize the size of sent data
 *
 * @return CSOAP_OK if it was successfull
 */
CSOAP_STATUS SoapSocketSend(HSOAPSOCKET sock, const char* req,ssize_t* sentSize);


/**
 * Receives data from the connected socket.
 *
 * @param sock A connected socket object
 * @param res destination pointer to receive 
 *  the data. This will be allocated in this 
 *  function. So you have to free it.
 * @param receivedSize the size of received data
 *
 * @return CSOAP_OK if it was successfull
 */
CSOAP_STATUS SoapSocketRecv(HSOAPSOCKET sock, char** res,ssize_t* receivedSize);


#endif


