/******************************************************************
 *  $Id: soap-router.c,v 1.1 2004/02/03 08:10:05 snowdrop Exp $
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
#include <libcsoap/soap-router.h>


SoapRouter *soap_router_new()
{
  SoapRouter *router;
  
  router = (SoapRouter*)malloc(sizeof(SoapRouter));
  router->service_head = NULL;
  router->service_tail = NULL;
  
  return router;
}


void soap_router_register_service(SoapRouter *router,
				  SoapServiceFunc func,
				  const char* method,
				  const char* urn)
{
  SoapService *service;
  
  service = soap_service_new(urn, method, func);
  
  if (router->service_tail == NULL) {
    router->service_head =
      router->service_tail = soap_service_node_new(service, NULL);
  } else {
    router->service_tail->next = 
      soap_service_node_new(service, NULL);
    router->service_tail = router->service_tail->next;
  }
}


SoapService* soap_router_find_service(SoapRouter *router, 
				      const char* urn,
				      const char* method)
{
  SoapServiceNode *node;

  if (router == NULL || urn == NULL || method == NULL) 
    return NULL;

  node = router->service_head;

  while (node) {
    if (node->service && node->service->urn 
	&& node->service->method) {
      
      if (!strcmp(node->service->urn, urn)
	  && !strcmp(node->service->method, method)) 
	return node->service;

    }

    node = node->next;
  }

  return NULL;
}


void soap_router_free(SoapRouter *router)
{
  SoapServiceNode *node;

  if (router == NULL) return;

  while (router->service_head) {
    node = router->service_head->next;
    soap_service_free(router->service_head->service);
    free(router->service_head);
    router->service_head = node;
  }

  free(router);
}
