/******************************************************************
 *  $Id: csoapenv.h,v 1.1 2003/03/25 22:17:19 snowdrop Exp $
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
#ifndef CSOAP_ENV_H
#define CSOAP_ENV_H

#include "csoapxml.h"
#include "csoapbody.h"

typedef HSOAPXML HSOAPENV;


/**
 * Creates a new soap envelope object
 * 
 * @param uri the URI 
 * @param method the method name to call
 *
 * @return handle to an envelope object if
 *  creation was successfull or NULL otherwise.
 */
HSOAPENV SoapEnvCreate(const char *uri,  const char *method);


/**
 * Creates a new soap envelope object
 * 
 * @param uri the URI 
 * @param method the method name to call
 *
 * @return handle to an envelope object if
 *  creation was successfull or NULL otherwise.
 */
HSOAPENV SoapEnvCreateWithFault(const char *uri,  const char *method);


/**
 * Returns the soap body object to 
 * a given envelope object.
 *
 * @param envelope the envelope
 * @return handle to a body object if
 * any exist or NULL otherwise
 */
HSOAPBODY SoapEnvGetBody(HSOAPENV envelope);


/**
 * Free a given envelope object
 *
 * @param envelope the envelope object
 *  to free
 */
void SoapEnvFree(HSOAPENV envelope);


/**
 * Dumps an envelope object to the stdout.
 * For debug puposes only.
 *
 * @param env the envelope object to dump
 */
void SoapEnvDump(HSOAPENV env);


#endif
