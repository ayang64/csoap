/******************************************************************
 *  $Id: csoapmethod.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_METHOD_H
#define CSOAP_METHOD_H

#include "libcsoap/csoapparam.h"


typedef HSOAPXML HSOAPMETHOD;


/**
 * Returns the method name
 *
 * @param method handle to a soap method object
 * @return the method name (do not free the string)
 */
char* SoapMethodGetName(HSOAPMETHOD method);


/**
 *  Returns the param context. Param context is
 * also a param object. Use the param context 
 * as the parent of the method parameters.
 *
 * @param method the method object to get
 *   its param context
 *
 * @return The root parameter object if any
 * exist , NULL otherwise.
 */
HSOAPPARAM SoapMethodParamContext(HSOAPMETHOD method);


#endif
