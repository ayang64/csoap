/******************************************************************
*  $Id: soap-client.c,v 1.32 2006/11/28 23:45:57 m0gg Exp $
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

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-addressing.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-transport.h"
#ifdef HAVE_XMLSEC1
#include "soap-xmlsec.h"
#endif
#include "soap-client.h"

herror_t
soap_client_init_args(int argc, char **argv)
{
  herror_t status;

#ifdef HAVE_XMLSEC1
  if ((status = soap_xmlsec_client_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_xmlsec_client_init_args failed (%s)", herror_message(status));
    return status;
  }
#endif

  if ((status = soap_transport_client_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_transport_client_init_args failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
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
  herror_t status;
  char *id;

  log_verbose2("action = \"%s\"", action);
  soap_addressing_set_action_string(req->env, action);

  log_verbose2("url = \"%s\"", url);
  soap_addressing_set_to_address_string(req->env, url);

  log_verbose2("from = \"%s\"", soap_transport_get_name());
  soap_addressing_set_from_address_string(req->env, soap_transport_get_name());

  soap_addressing_set_message_id_string(req->env, NULL);
  id = soap_addressing_get_message_id_string(req->env);
  log_verbose2("generated message id = \"%s\"", id);
  free(id);

#ifdef HAVE_XMLSEC1
  log_error1("trying encryption and signation");

  if ((status = soap_xmlsec_encrypt(req)) != H_OK)
  {
    log_error2("soap_xmlsec_encrypt failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_xmlsec_sign(req)) != H_OK)
  {
    log_error2("soap_xmlsec_sign failed (%s)", herror_message(status));
    return status;
  }
#endif

  if ((status = soap_transport_client_invoke(req, res)) != H_OK)
  {
    log_error2("soap_transport_client_invoke failed (%s)", herror_message(status));
    return status;
  }

#ifdef HAVE_XMLSEC1
  if ((status = soap_xmlsec_verify(*res)) != H_OK)
  {
    log_error2("soap_xmlsec_verify failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_xmlsec_decrypt(*res)) != H_OK)
  {
    log_error2("soap_xmlsec_decrypt failed (%s)", herror_message(status));
    return status;
  }
#endif

  return H_OK;
}
