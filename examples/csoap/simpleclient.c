/******************************************************************
 * $Id: simpleclient.c,v 1.20 2006/11/29 11:04:24 m0gg Exp $
 *
 * CSOAP Project:  CSOAP examples project 
 * Copyright (C) 2003-2004  Ferhat Ayaz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include <libcsoap/soap-ctx.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-addressing.h>
#include <libcsoap/soap-client.h>

static char *url = "http://localhost:10000/csoapserver";
static char *urn = "urn:examples";
static char *method = "sayHello";

int
main(int argc, char **argv)
{
  struct SoapCtx *request;
  struct SoapCtx *response;
  herror_t err;

  // hlog_set_level(HLOG_VERBOSE);

  err = soap_client_init_args(argc, argv);
  if (err != H_OK)
  {
    printf("%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }

  err = soap_ctx_new_with_method(urn, method, &request);
  if (err != H_OK)
  {
    printf("%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }

  soap_env_add_item(request->env, "xsd:string", "name", "Jonny B. Good");

  if (argc > 1)
    url = argv[1];

  printf("**** sending to \"%s\" ****\n", url);
  xmlDocFormatDump(stderr, request->env->root->doc, 1);

  if ((err = soap_client_invoke(request, &response, url, "")) != H_OK)
  {
    printf("[%d] %s(): %s\n", herror_code(err), herror_func(err), herror_message(err));
    herror_release(err);
    soap_ctx_free(request);
    exit(1);
  }

  printf("**** received from \"%s\" ****\n", soap_addressing_get_from_address_string(response->env));
  xmlDocFormatDump(stdout, response->env->root->doc, 1);

  soap_ctx_free(response);
  soap_ctx_free(request);

  soap_client_destroy();

  return 0;
}
