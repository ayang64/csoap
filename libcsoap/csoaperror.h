/******************************************************************
 *  $Id: csoaperror.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_ERROR_H
#define CSOAP_ERROR_H

typedef int CSOAP_STATUS;

/* Ok status */
#define CSOAP_OK 0

/* Socket related errors */
#define ERROR_SOCKET_CAN_NOT_GET_HOSTNAME   100001
#define ERROR_SOCKET_CAN_NOT_CONNECT   100002
#define ERROR_SOCKET_CAN_NOT_CREATE_SOCKET   100003
#define ERROR_SOCKET_CAN_NOT_RECEIVE   100004
#define ERROR_SOCKET_CAN_NOT_SEND   100005


/* Generic errors */
#define ERROR_NO_MEM   110001


/* xml related error */
#define ERROR_XML_CAN_NOT_PARSE 12001

#endif
