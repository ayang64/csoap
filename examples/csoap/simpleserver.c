/******************************************************************
 * $Id: simpleserver.c,v 1.26 2006/11/26 20:13:05 m0gg Exp $
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
#include <libxml/xpath.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include <libcsoap/soap-xml.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-ctx.h>
#include <libcsoap/soap-service.h>
#include <libcsoap/soap-router.h>
#include <libcsoap/soap-server.h>

static const char const *url = "/csoapserver";
static const char const *urn = "urn:examples";
static const char const *method = "sayHello";

herror_t
say_hello(struct SoapCtx *req, struct SoapCtx *res)
{
  herror_t err;
  char *name;
  xmlNodePtr method, node;

  printf("processing service request\n");

  xmlDocFormatDump(stdout, req->env->root->doc, 1);

  err = soap_env_new_with_response(req->env, &res->env);
  if (err != H_OK)
  {
    printf("soap_env_new_with_response failed (%s)\n", herror_message(err));
    return err;
  }
  printf("empty response created\n");

  if (!(method = soap_env_get_method(req->env)))
  {
    printf("soap_env_get_method failed\n");
    return herror_new("say_hello", 0, "There may be a bug in the library...");
  }
  printf("method found\n");

  printf("adding response content...\n");
  node = soap_xml_get_children(method);
  while (node)
  {
    name = (char *) xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
    soap_env_add_itemf(res->env, "xsd:string", "echo", "Hello '%s'", name);
    node = soap_xml_get_next(node);
    if (name)
      xmlFree(name);
  }

  printf("service request done\n");

  return H_OK;
}

int
main(int argc, char **argv)
{

  herror_t err;
  struct SoapRouter *router;

  hlog_set_level(HLOG_VERBOSE);

  if ((err = soap_server_init_args(argc, argv)) != H_OK)
  {
    printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }

  if (!(router = soap_router_new()))
  {
    printf("soap_router_new failed (%p)\n", router);
    herror_release(err);
    exit(1);
  }

  if ((err = soap_router_register_service(router, say_hello, method, urn)) != H_OK)
  {
    printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }

  if ((err = soap_server_register_router(router, url)))
  {
    printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }
  printf("router (%p) registered for \"%s\"\n", router, url);

  printf("press ctrl-c to shutdown\n");
  if ((err = soap_server_run()) != H_OK)
  {
    printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    exit(1);
  }
  
  printf("shutting down\n");
  soap_server_destroy();

  return 0;
}
