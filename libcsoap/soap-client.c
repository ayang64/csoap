/******************************************************************
*  $Id: soap-client.c,v 1.34 2007/11/03 22:40:09 m0gg Exp $
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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-error.h>

#include "soap-logging.h"
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

#ifdef HAVE_XMLSEC1
static int _soap_client_force_sign = 0;
static int _soap_client_force_verify = 0;
static int _soap_client_force_encrypt = 0;
static int _soap_client_force_decrypt = 0;

static inline void
_soap_client_parse_arguments(int argc, char **argv)
{
  int i;

  for (i=0; i<argc; i++)
  {
    if (!strcmp(CSOAP_CLIENT_FORCE_ENCRYPT, argv[i]))
    {
      _soap_client_force_encrypt = 1;
    }
    else if (!strcmp(CSOAP_CLIENT_FORCE_DECRYPT, argv[i]))
    {
      _soap_client_force_decrypt = 1;
    }
    else if (!strcmp(CSOAP_CLIENT_FORCE_VERIFY, argv[i]))
    {
      _soap_client_force_verify = 1;
    }
    else if (!strcmp(CSOAP_CLIENT_FORCE_SIGN, argv[i]))
    {
      _soap_client_force_sign = 1;
    }
  }
  return;
}
#endif

herror_t
soap_client_init_args(int argc, char **argv)
{
  herror_t status;

#ifdef HAVE_XMLSEC1
  _soap_client_parse_arguments(argc, argv);

  if ((status = soap_xmlsec_client_init_args(argc, argv)) != H_OK)
  {
    log_error("soap_xmlsec_client_init_args failed (%s)", herror_message(status));
    return status;
  }
#endif

  if ((status = soap_transport_client_init_args(argc, argv)) != H_OK)
  {
    log_error("soap_transport_client_init_args failed (%s)", herror_message(status));
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

  log_verbose("action = \"%s\"", action);
  soap_addressing_set_action_string(req->env, action);

  log_verbose("url = \"%s\"", url);
  soap_addressing_set_to_address_string(req->env, url);

  log_verbose("from = \"%s\"", soap_transport_get_name());
  soap_addressing_set_from_address_string(req->env, soap_transport_get_name());

  soap_addressing_set_message_id_string(req->env, NULL);
  id = soap_addressing_get_message_id_string(req->env);
  log_verbose("generated message id = \"%s\"", id);
  free(id);

#ifdef HAVE_XMLSEC1
  if ((status = soap_xmlsec_encrypt(req)) != H_OK)
  {
    log_error("soap_xmlsec_encrypt failed (%s)", herror_message(status));
    if (_soap_client_force_encrypt)
      return status;
  }
  else
  {
    log_verbose("soap_xmlsec_encrypt succeed");
  }

  if ((status = soap_xmlsec_sign(req)) != H_OK)
  {
    log_error("soap_xmlsec_sign failed (%s)", herror_message(status));
    if (_soap_client_force_sign)
      return status;
  }
  else
  {
    log_verbose("soap_xmlsec_encrypt succeed");
  }
#endif

  if ((status = soap_transport_client_invoke(req, res)) != H_OK)
  {
    log_error("soap_transport_client_invoke failed (%s)", herror_message(status));
    return status;
  }

#ifdef HAVE_XMLSEC1
  if ((status = soap_xmlsec_verify(*res)) != H_OK)
  {
    log_error("soap_xmlsec_verify failed (%s)", herror_message(status));
    if (_soap_client_force_verify)
      return status;
  }
  else
  {
    log_verbose("soap_xmlsec_verify succeed");
  }

  if ((status = soap_xmlsec_decrypt(*res)) != H_OK)
  {
    log_error("soap_xmlsec_decrypt failed (%s)", herror_message(status));
    if (_soap_client_force_decrypt)
      return status;
  }
  else
  {
    log_verbose("soap_xmlsec_decrypt succeed");
  }
#endif

  return H_OK;
}
