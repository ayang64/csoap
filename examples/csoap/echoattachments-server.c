/******************************************************************
 * $Id: echoattachments-server.c,v 1.2 2004/10/15 15:10:15 snowdrop Exp $
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

#include <libcsoap/soap-server.h>


static const char *url = "/echoattachment";
static const char *urn = "";
static const char *method = "echo";



SoapCtx* echo_attachments(SoapCtx *req)
{

  SoapEnv *env;
  SoapCtx*  ctx;
  part_t *part;
  char href[MAX_HREF_SIZE];

  env = soap_env_new_with_response(req->env);
  ctx = soap_ctx_new(env);

  if (req->attachments) {
    part = req->attachments->parts;
    while (part) {
      log_verbose2("Adding part '%s'", part->filename);
      soap_ctx_add_file(ctx, part->filename, "text/plain", href);
      soap_env_add_attachment(ctx->env, "echoFile", href);
      part = part->next;
    }
  }
    
  return ctx;
}


int main(int argc, char *argv[])
{

  SoapRouter *router;
  
  log_set_level(HLOG_VERBOSE);

  if (!soap_server_init_args(argc, argv)) {
    return 1;
  }
  
  router = soap_router_new();
  soap_router_register_service(router, echo_attachments, method, urn);
  soap_server_register_router(router, url);

  log_info1("send SIGTERM to shutdown");
  soap_server_run();

  log_info1("shutting down\n"); 
  soap_server_destroy();

  return 0;
}

