/******************************************************************
 *  $Id: soap-call.h,v 1.1 2004/01/30 16:39:34 snowdrop Exp $
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
#ifndef cSOAP_CALL_H
#define cSOAP_CALL_H

#include <libcsoap/soap-xml.h>


typedef struct _SoapCall
{
  xmlNodePtr root; /* shortcut */
  xmlNodePtr cur;
}SoapCall;



SoapCall* soap_call_new(const char *urn, const char *method);

xmlNodePtr
soap_call_add_param(SoapCall *call, const char *type, 
		    const char *name, const char *value);

xmlNodePtr
soap_call_add_paramf(SoapCall *call, const char *type,
		     const char *name, const char *format, ...);


xmlNodePtr
soap_call_push_param(SoapCall *call, const char *type,
		     const char *name);

void
soap_call_pop_param(SoapCall *call);


xmlDocPtr
soap_call_invoke(SoapCall *call, const char *url);


#endif


