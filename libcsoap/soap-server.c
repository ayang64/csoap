/******************************************************************
*  $Id: soap-server.c,v 1.30 2006/11/23 15:27:33 m0gg Exp $
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
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-server.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-addressing.h"
#include "soap-transport.h"

#include "soap-server.h"

static SoapRouterNode *head = NULL;
static SoapRouterNode *tail = NULL;

static SoapRouterNode *
router_node_new(struct SoapRouter * router, const char *context, SoapRouterNode * next)
{
  const char *noname = "/lost_found";
  SoapRouterNode *node;

  if (!(node = (SoapRouterNode *) malloc(sizeof(SoapRouterNode))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  if (context)
  {
    node->context = strdup(context);
  }
  else
  {
    log_warn2("context is null, using '%s'", noname);
    node->context = strdup(noname);
  }

  node->router = router;
  node->next = next;

  return node;
}

static herror_t
_soap_server_env_new_with_fault(const char *fault_string, const char *detail, struct SoapEnv **out)
{
  return soap_env_new_with_fault(SOAP_FAULT_RECEIVER, fault_string, soap_server_get_name(), detail, out);
}

struct SoapRouter *
soap_server_find_router(const char *context)
{
  SoapRouterNode *node;

  for (node = head; node; node = node->next)
  {
    if (!strcmp(node->context, context))
      return node->router;
  }

  return NULL;
}

herror_t
soap_server_process(struct SoapCtx *request, struct SoapCtx **response)
{
  char buffer[1054];
  const char *urn;
  const char *method;
  char *to;
  struct SoapRouter *router;
  SoapService *service;
  herror_t err;

  log_verbose1("processing");
  xmlDocDump(stdout, request->env->root->doc);

  *response = soap_ctx_new(NULL);

  if (!(method = soap_env_find_methodname(request->env)))
  {
    _soap_server_env_new_with_fault("No method found", "The method is missing in the SOAP envelope", &((*response)->env));
    return H_OK;
  }
  log_verbose2("method: \"%s\"", method);

  if (!(urn = soap_env_find_urn(request->env)))
  {
    _soap_server_env_new_with_fault("No URN found", "The URN is missing in the SOAP envelope", &((*response)->env));
    return H_OK;
  }
  log_verbose2("urn: \"%s\"", urn);

  if ((to = soap_addressing_get_to_address_string(request->env)))
  {
    if (!(router = soap_server_find_router(to)))
    {
      sprintf(buffer, "no router for context \"%s\" found", to);
      _soap_server_env_new_with_fault(buffer, "The method is unknown by the server", &((*response)->env));
      free(to);
      return H_OK;
    }
    free(to);
  }
  else
  {
    _soap_server_env_new_with_fault(buffer, "The destination address is missing", &((*response)->env));
    return H_OK;
  }
  log_verbose2("router: %p", router);

  if (!(service = soap_router_find_service(router, urn, method)))
  {
    sprintf(buffer, "no service for URN \"%s\" found", urn);
    _soap_server_env_new_with_fault(buffer, "The URN is not known by the server", &((*response)->env));
    return H_OK;
  }
  log_verbose2("service found (%p)", service);

  log_verbose2("service function: %p", service->func);
  if ((err = service->func(request, *response)) != H_OK)
  {
    sprintf(buffer, "Service returned following error message: \"%s\"", herror_message(err));
    herror_release(err);
    _soap_server_env_new_with_fault("Internal service error", buffer, &((*response)->env));
    return H_OK;
  }

  if ((*response)->env == NULL)
  {
    sprintf(buffer, "Service \"%s\" returned no envelope", urn);
    _soap_server_env_new_with_fault("Internal service error", buffer, &((*response)->env));
    return H_OK;
  }

  return H_OK;
}

herror_t
soap_server_init_args(int argc, char **argv)
{
  herror_t status;

  if ((status = soap_transport_server_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_transport_server_init_args failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

const char *
soap_server_get_name(void)
{
  return soap_transport_get_name();
}

herror_t
soap_server_register_router(struct SoapRouter *router, const char *context)
{
  herror_t status;

  if ((status = soap_transport_register(context)) != H_OK)
  {
    log_error2("soap_transport_register failed (%s)", herror_message(status));
    return status;
  }

  if (tail == NULL)
  {
    head = tail = router_node_new(router, context, NULL);
  }
  else
  {
    tail->next = router_node_new(router, context, NULL);
    tail = tail->next;
  }

  return H_OK;
}

SoapRouterNode *
soap_server_get_routers(void)
{
  return head;
}

herror_t
soap_server_run(void)
{
  herror_t status;

  if ((status = soap_transport_server_run()) != H_OK)
    return status;

  return H_OK;
}

void
soap_server_destroy(void)
{
  SoapRouterNode *node = head;
  SoapRouterNode *tmp;

  while (node != NULL)
  {
    tmp = node->next;
    log_verbose2("soap_router_free(%p)", node->router);
    soap_router_free(node->router);
    free(node->context);
    free(node);
    node = tmp;
  }
  
  soap_transport_server_destroy();

  return;
}
