/******************************************************************
 *  $Id: csoapcall.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_CALL_H
#define CSOAP_CALL_H

#include "libcsoap/csoapparam.h"
#include "libcsoap/csoapres.h"
#include "libcsoap/csoapenv.h"
#include "libcsoap/csoapurl.h"

typedef struct SOAPCALL* HSOAPCALL;


/**
 * Stores information to 
 * send a soap call
 */
struct SOAPCALL
{
  HSOAPENV m_env; /* The soap envelope */
  HSOAPURL m_url;  /* The target URL */
  char* m_method; /* The method to to call */
};


HSOAPCALL SoapCallCreate(
   const char* url,
   const char* uri,
   const char* method
);


HSOAPPARAM SoapCallAddParam(
   HSOAPCALL call,
   const char* name,
   const char* type,
   const char* format,
   ...
);
   

HSOAPPARAM SoapCallAddStringParam(
   HSOAPCALL call,
   const char* name,
   const char* format,
   ...
);


HSOAPPARAM SoapCallAddIntParam(
   HSOAPCALL call,
   const char* name,
   int value
);


HSOAPPARAM SoapCallAddDoubleParam(
   HSOAPCALL call,
   const char* name,
   double value
);


HSOAPRES SoapCallInvoke(HSOAPCALL call);

void SoapCallFree(HSOAPCALL call);


#endif
