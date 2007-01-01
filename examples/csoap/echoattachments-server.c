/******************************************************************
 * $Id: echoattachments-server.c,v 1.13 2007/01/01 22:54:46 m0gg Exp $
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

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include <libcsoap/soap-env.h>
#include <libcsoap/soap-ctx.h>
#include <libcsoap/soap-service.h>
#include <libcsoap/soap-router.h>
#include <libcsoap/soap-server.h>

static const char *url = "/echoattachments";
static const char *urn = "urn:examples";
static const char *method = "echo";

herror_t
echo_attachments(struct SoapCtx * req, struct SoapCtx * res)
{
  herror_t err;

  struct part_t *part;
  char href[MAX_HREF_SIZE];

  err = soap_env_new_with_response(req->env, &res->env);
  if (err != H_OK)
  {
    return err;
  }

  if (req->attachments)
  {
    for (part = req->attachments->parts; part != NULL; part = part->next)
    {
      printf("attaching \"%s\"\n", part->filename);
      soap_ctx_add_file(res, part->filename, part->content_type, href);
      soap_env_add_attachment(res->env, "echoFile", href);
    }
  }

  return H_OK;
}

int
main(int argc, char **argv)
{
  herror_t err;
  struct SoapRouter *router;

  hlog_set_level(HLOG_VERBOSE);

  err = soap_server_init_args(argc, argv);
  if (err != H_OK)
  {
    printf("%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    return 1;
  }

  router = soap_router_new();
  soap_router_register_service(router, echo_attachments, method, urn);
  soap_server_register_router(router, url);

  printf("send SIGTERM to shutdown");
  soap_server_run();

  printf("shutting down\n");
  soap_server_destroy();

  return 0;
}
