/******************************************************************
 *  $Id: csoapbody.h,v 1.1 2003/03/25 22:17:18 snowdrop Exp $
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
#ifndef CSOAP_BODY_H
#define CSOAP_BODY_H

#include "csoapmethod.h"
#include "csoapfault.h"

typedef HSOAPXML HSOAPBODY;



/**
 *  Returns the method object assigned to 
 * the given body. 
 *
 * @param body handle to the body object
 * @param method the method(operation) name 
 *  to call.
 *
 * @return a handle to the method object or
 *  NULL if the the method was not found.
 */
HSOAPMETHOD SoapBodyGetMethod(HSOAPBODY body, const char *method);


/**
 * Returns the fault object if any exist.
 *
 * @param body handle to the body object
 * @return a handle to a fault object or 
 * NULL if no fault was generated.
 */
HSOAPFAULT SoapBodyGetFault(HSOAPBODY body);


#endif
