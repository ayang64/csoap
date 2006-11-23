/******************************************************************
*  $Id: soap-client.c,v 1.30 2006/11/23 15:27:33 m0gg Exp $
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

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-server.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-addressing.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-transport.h"
#include "soap-client.h"

herror_t
soap_client_init_args(int argc, char **argv)
{
  return soap_transport_client_init_args(argc, argv);
}

void
soap_client_destroy(void)
{
  soap_transport_client_destroy();
  
  return;
}

herror_t
soap_client_invoke(struct SoapCtx *req, struct SoapCtx **res, const char *url, const char *action)
{
  char *id;

  log_verbose2("action = \"%s\"", action);
  soap_addressing_set_action_string(req->env, action);

  log_verbose2("url = \"%s\"", url);
  soap_addressing_set_to_address_string(req->env, url);

  soap_addressing_set_message_id_string(req->env, NULL);
  id = soap_addressing_get_message_id_string(req->env);
  log_verbose2("message id = \"%s\"", id);
  free(id);

  return soap_transport_client_invoke(req, res);
}
