/******************************************************************
*  $Id: soap-nudp.c,v 1.2 2006/11/23 15:27:33 m0gg Exp $
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

#ifdef HAVE_ERRNO_H
#include <errno.h>
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
#include "soap-server.h"
#include "soap-addressing.h"

herror_t
soap_nudp_server_init_args(int argc, char **argv)
{
  return H_OK;
}

herror_t
soap_nudp_register(const void *data)
{
  return H_OK;
}

herror_t
soap_nudp_server_run(void)
{
  return H_OK;
}

void
soap_nudp_server_destroy(void)
{
  return;
}

herror_t
soap_nudp_client_init_args(int argc, char **argv)
{
  return H_OK;
}

herror_t
soap_nudp_client_invoke(struct SoapCtx *req, struct SoapCtx **res)
{
  return H_OK;
}

void
soap_nudp_client_destroy(void)
{
  return;
}
