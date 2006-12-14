/******************************************************************
*  $Id: soap-transport.c,v 1.9 2006/12/14 19:36:49 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2007 Heiko Ronsdorf
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
* Email: hero@persua.de
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-addressing.h"
#include "soap-server.h"

#include "soap-nhttp.h"
#include "soap-nudp.h"

#include "soap-transport.h"

struct soap_transport
{
  char *scheme;
  void *data;
  msg_exchange invoke;
  struct soap_transport *next;
};

static struct soap_transport *head = NULL;
static char soap_transport_name[512] = "not set";

static struct soap_transport *
_soap_transport_new(const char *scheme, void *data, msg_exchange invoke)
{
  struct soap_transport *ret;

  if (!(ret = (struct soap_transport *)malloc(sizeof(struct soap_transport))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  memset(ret, 0, sizeof(struct soap_transport));

  ret->scheme = strdup(scheme);
  ret->data = data;
  ret->invoke = invoke;

  log_verbose4("scheme=\"%s\", data=%p, invoke=%p", ret->scheme, ret->data, ret->invoke);

  return ret;
}

/* static void *
_soap_transport_destroy(struct soap_transport *transport)
{
  void *ret;

  if (transport->scheme)
    free(transport->scheme);

  ret = transport->data;

  free(transport);

  return ret;
} */

herror_t
soap_transport_process(struct SoapCtx *request, struct SoapCtx **response)
{
  return soap_server_process(request, response);
}

static herror_t
_soap_transport_set_name(void)
{
  static int set = 0;
  char hostname[256];

  if (set)
    return H_OK;

  gethostname(hostname, 256);
  sprintf(soap_transport_name, "%s://%s:%i", soap_nhttp_get_protocol(), hostname, soap_nhttp_get_port());

  set = 1;

  return H_OK;
}

herror_t
soap_transport_server_init_args(int argc, char **argv)
{
  herror_t status;

  if ((status = soap_nhttp_server_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_nhttp_server_init_args failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_nudp_server_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_nudp_server_init_args failed (%s)", herror_message(status));
    return status;
  }

  if ((status = _soap_transport_set_name()) != H_OK)
  {
    log_error2("_soap_transport_set_name failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
soap_transport_register(const void *data)
{
  herror_t status;

  if ((status = soap_nhttp_register(data)) != H_OK)
  {
    log_error2("soap_nhttp_register failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_nudp_register(data)) != H_OK)
  {
    log_error2("soap_nudp_register failed (%s)", herror_message(status));
    return status;
  }
  
  return H_OK;
}

herror_t
soap_transport_add(const char *scheme, void *data, msg_exchange invoke)
{
  struct soap_transport *transport;
  struct soap_transport *walker;

  if (!(transport = _soap_transport_new(scheme, data, invoke)))
  {
    log_error1("_soap_transport_new failed");
    return herror_new("soap_transport_add", 0, "_soap_transport_new failed");
  }

  if (head == NULL)
  {
    head = transport;
  }
  else
  {
    for (walker=head; walker->next; walker=head->next);
      /* nothing */
    walker->next = transport;
  }
  return H_OK;
}

herror_t
soap_transport_server_run(void)
{
  herror_t status;

  if ((status = soap_nudp_server_run_threaded()) != H_OK)
  {
    log_error2("soap_nudp_server_run failed (%s)", herror_message(status));
    return status;
  }

  /* nanoHTTP blocks in this call */
  if ((status = soap_nhttp_server_run()) != H_OK)
  {
    log_error2("soap_nhttp_server_run failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}
 
const char *
soap_transport_get_name(void)
{
  return soap_transport_name;
}

void
soap_transport_server_destroy(void)
{
  soap_nhttp_server_destroy();

  soap_nudp_server_destroy();
  
  return;
}

herror_t
soap_transport_client_init_args(int argc, char **argv)
{
  herror_t status;

  if ((status = soap_nhttp_client_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_nhttp_client_init_args failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_nudp_client_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_nudp_client_init_args failed (%s)", herror_message(status));
    return status;
  }

  if ((status = _soap_transport_set_name()) != H_OK)
  {
    log_error2("_soap_transport_set_name failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
soap_transport_client_invoke(struct SoapCtx *request, struct SoapCtx **response)
{
  struct soap_transport *walker;
  herror_t ret;
  xmlURI *dest;
  
  /* log_verbose1(__FUNCTION__);
   xmlDocFormatDump(stdout, request->env->root->doc, 1); */

  if (!(dest = soap_addressing_get_to_address(request->env)))
  {
    log_verbose1("soap_addressing_get_to_address failed");
    return herror_new("soap_transport_client_invoke", 0, "cannot find to address in SOAP envelope");
  }

  if (!dest->scheme)
  {
    log_verbose1("missing scheme (protocol) in to address");
    return herror_new("soap_transport_client_invoke", 0, "cannot find protocol in destination address");
  }

  log_verbose2("trying to contact \"%s\"", soap_addressing_get_to_address_string(request->env));

  for (walker=head; walker; walker=walker->next)
  {
    log_verbose3("testing transport server \"%s\" for \"%s\"", walker->scheme, dest->scheme);
    if (!strcmp(walker->scheme, dest->scheme))
    {
      log_verbose3("found transport layer for \"%s\" (%p)", dest->scheme, walker->invoke);
      ret = walker->invoke(walker->data, request, response);
      xmlFreeURI(dest);
      return ret;
    }
  }

  ret = herror_new("soap_transport_client_invoke", 0, "no transport service found for \"%s\"", dest->scheme);
  xmlFreeURI(dest);

  return ret;
}

void
soap_transport_client_destroy(void)
{
  soap_nhttp_client_destroy();

  soap_nudp_client_destroy();

  return;
}
