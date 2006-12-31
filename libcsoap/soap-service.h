/******************************************************************
 *  $Id: soap-service.h,v 1.10 2006/12/31 17:24:22 m0gg Exp $
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
 * Email: ferhatayaz@jprogrammer.net
 ******************************************************************/
#ifndef __csoap_service_h
#define __csoap_service_h

/** @file
 *
 * A Web service is a software system designed to support interoperable
 * machine-to-machine interaction over a network. It has an interface described
 * in a machine-processable format (specifically WSDL). Other systems interact
 * with the Web service in a manner prescribed by its description using SOAP
 * messages, typically conveyed using HTTP with an XML serialization in
 * conjunction with other Web-related standards.
 *
 * @see http://www.w3.org/TR/wslc/
 * @see http://www.w3.org/TR/wsdl
 * @see http://www.w3.org/TR/owl-ref/
 *
 */

/**
 *
 * This service status shows that the provider agent is not capable of accepting
 * any requests (i.e. the service is not available).
 *
 * @see http://www.w3.org/TR/wslc/
 *
 */
#define CSOAP_SERVICE_DOWN	0

/**
 *
 * This service status shows that the provider agent is capable of accepting and
 * processing requests (i.e. the service is available)
 *
 * @see http://www.w3.org/TR/wslc/
 *
 */
#define CSOAP_SERVICE_UP	1

typedef herror_t (*SoapServiceFunc)(struct SoapCtx *request, struct SoapCtx *response);

typedef struct _SoapService
{
  char *urn;
  char *method;
  int status;
  SoapServiceFunc func;
} SoapService;


typedef struct _SoapServiceNode
{
  SoapService *service;
  struct _SoapServiceNode *next;
} SoapServiceNode;


#ifdef __cplusplus
extern "C" {
#endif

extern SoapServiceNode *soap_service_node_new(SoapService * service, SoapServiceNode * next);

extern SoapService *soap_service_new(const char *urn, const char *method, SoapServiceFunc f);

extern void soap_service_free(SoapService * service);

#ifdef __cplusplus
}
#endif

#endif
