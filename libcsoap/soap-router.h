/******************************************************************
 *  $Id: soap-router.h,v 1.1 2004/02/03 08:10:05 snowdrop Exp $
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
#ifndef cSOAP_ROUTER_H
#define cSOAP_ROUTER_H


#include <libcsoap/soap-service.h>

typedef struct _SoapRouter
{
  SoapServiceNode *service_head;
  SoapServiceNode *service_tail;
}SoapRouter;


SoapRouter *soap_router_new();

void soap_router_register_service(SoapRouter *router,
				  SoapServiceFunc func,
				  const char* method,
				  const char* urn);


SoapService* soap_router_find_service(SoapRouter *router, 
				      const char* urn,
				      const char* method);

void soap_router_free(SoapRouter *router);

#endif
