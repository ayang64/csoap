/******************************************************************
*  $Id: soap-service.c,v 1.11 2006/11/23 15:27:33 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"

SoapServiceNode *
soap_service_node_new(SoapService * service, SoapServiceNode * next)
{
  SoapServiceNode *node;

  if (!(node = (SoapServiceNode *) malloc(sizeof(SoapServiceNode)))) {

    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }
  node->service = service;
  node->next = next;

  return node;
}

SoapService *
soap_service_new(const char *urn, const char *method, SoapServiceFunc f)
{
  SoapService *service;

  if (!(service = (SoapService *) malloc(sizeof(SoapService)))) {

    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  service->func = f;

  if (urn == NULL) {

    log_warn1("urn is NULL");
    urn = "";
  }
  service->urn = strdup(urn);

  if (method == NULL) {

    log_warn1("method is NULL");
    method = "";
  }
  service->method = strdup(method);

  return service;
}

void
soap_service_free(SoapService * service)
{
  if (!service)
    return;

  if (service->urn)
	  free(service->urn);

  if (service->method)
	  free(service->method);

  free(service);

  return;
}
