/******************************************************************
 *  $Id: csoapres.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_RES_H
#define CSOAP_RES_H

#include "libcsoap/csoapxml.h"
#include "libcsoap/csoapenv.h"

typedef struct SOAPRES* HSOAPRES;


/**
 * The result object. Stores
 * the response of a soap request.
 */
struct SOAPRES 
{
  HSOAPENV m_env; /* The soap envelope object */
  char *m_method; /* The called method name */
};


/**
 * Creates a new soap result object.
 * This is used actually internally.
 * To get a soap result see 
 * <B>HSOAPCALL</B>
 * 
 * @param env the envelope object to use
 * @param methodName the called method name
 * @return handle to a soap result object.
 */
HSOAPRES SoapResCreate(HSOAPENV env, const char* methodName);


/**
 * Returns a soap fault object
 * if any exist. This can also be
 * called to check if any faults
 * was occured for this result.
 *
 * @param res handle to the soap result object
 * @return handle to a fault object or
 *  NULL if no fault was generated.
 */
HSOAPFAULT SoapResGetFault(HSOAPRES res);


/**
 * Returns the first parameter
 * in the result object if any exist.
 *
 * @param res handle to the soap result object
 * @return handle to a soap param object
 *  NULL if no parameter was received.
 */
HSOAPPARAM SoapResGetParamChildren(HSOAPRES res);


/**
 * Free a soap result object.
 * This must be called after the result
 * object was used.
 *
 * @param res The soap result object to free
 */
void SoapResFree(HSOAPRES res);

#endif
