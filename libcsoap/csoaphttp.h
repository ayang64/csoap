/******************************************************************
 *  $Id: csoaphttp.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_HTTP_H
#define CSOAP_HTTP_H

#include "libcsoap/csoapenv.h"
#include "libcsoap/csoapsocket.h"
#include "libcsoap/csoapurl.h"

/**
 * Sends a soap envelope to the given 
 * host and receives a soap envelope.
 *
 * @param url The target url
 * @param env The soap envelope to send
 * @param res The destination soap handle to 
 *   receive the the envelope.
 *
 * @return CSOAP_OK if it was successfull.
 */
CSOAP_STATUS SoapHttpSend(HSOAPURL url, HSOAPENV env, HSOAPENV* res);


#endif
