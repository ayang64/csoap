/******************************************************************
*  $Id: soap-router.c,v 1.13 2006/11/23 15:27:33 m0gg Exp $
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

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <libxml/tree.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-ctx.h"
#include "soap-service.h"

#include "soap-router.h"

struct SoapRouter *
soap_router_new(void)
{
  struct SoapRouter *router;

  if (!(router = (struct SoapRouter *) malloc(sizeof(struct SoapRouter))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }
  memset(router, 0, sizeof(struct SoapRouter));

  return router;
}

herror_t
soap_router_register_service(struct SoapRouter *router, SoapServiceFunc func, const char *method, const char *urn)
{
  SoapService *service;

  log_verbose4("registering service (router=%p, method=\"%s\", urn=\"%s\")", router, method, urn);

  if (!(service = soap_service_new(urn, method, func)))
    return herror_new("soap_router_register_service", 0, "soap_service_new failed");

  if (router->service_tail == NULL)
  {
    router->service_head =
      router->service_tail = soap_service_node_new(service, NULL);
  }
  else
  {
    router->service_tail->next = soap_service_node_new(service, NULL);
    router->service_tail = router->service_tail->next;
  }

  return H_OK;
}

void
soap_router_register_security(struct SoapRouter * router, soap_auth auth)
{
  router->auth = auth;

  return;
}

void
soap_router_register_description(struct SoapRouter * router, xmlDocPtr description)
{
  if (router->description)
    xmlFreeDoc(router->description);

  router->description = xmlCopyDoc(description, 1);

  return;
}

herror_t
soap_router_register_default_service(struct SoapRouter *router, SoapServiceFunc func, const char *method, const char *urn) {

  SoapService *service;

  if (!(service = soap_service_new(urn, method, func)))
    return herror_new("soap_router_register_default_service", 0, "soap_service_new failed");

  if (router->service_tail == NULL)
  {
    router->service_head = router->service_tail = soap_service_node_new(service, NULL);
  }
  else
  {
    router->service_tail->next = soap_service_node_new(service, NULL);
    router->service_tail = router->service_tail->next;
  }

  router->default_service = service;

  return H_OK;
}

SoapService *
soap_router_find_service(struct SoapRouter *router, const char *urn, const char *method)
{
  SoapServiceNode *node;

  if (router == NULL)
  {
    log_verbose1("router is null");
    return NULL;
  }

  if (urn == NULL)
  {
    log_verbose1("URN is null");
    return NULL;
  }

  if (method == NULL)
  {
    log_verbose1("method is null"); 
    return NULL;
  }

  log_verbose2("router = %p", router);
  log_verbose2("router->service_head = %p", router->service_head);

  node = router->service_head;

  while (node)
  {
    if (node->service && node->service->urn && node->service->method)
    {
      log_verbose4("checking service (node=%p, method=\"%s\", urn=\"%s\")", node->service, node->service->method, node->service->urn);
      if (!strcmp(node->service->urn, urn) && !strcmp(node->service->method, method))
        return node->service;
    }
    node = node->next;
  }
  return router->default_service;
}

void
soap_router_free(struct SoapRouter * router)
{
  SoapServiceNode *node;
  log_verbose2("enter: router=%p", router);

  if (!router)
    return;

  while (router->service_head)
  {
    node = router->service_head->next;
    /* log_verbose2("soap_service_free(%p)\n",
       router->service_head->service); */
    soap_service_free(router->service_head->service);
    free(router->service_head);
    router->service_head = node;
  }
  if (router->description)
    xmlFreeDoc(router->description);

  free(router);
  log_verbose1("leave with success");

  return;
}
