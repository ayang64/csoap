/******************************************************************
 * $Id: simpleserver.c,v 1.4 2004/08/30 13:28:58 snowdrop Exp $
 *
 * CSOAP Project:  CSOAP examples project 
 * Copyright (C) 2003  Ferhat Ayaz
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/

#include <libcsoap/soap-server.h>


static const char *url = "/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";


void add_name(xmlNodePtr node, SoapEnv *env)
{
  char *name;
  name = (char*)xmlNodeListGetString(node->doc, 
				     node->xmlChildrenNode, 1);


  if (!name) return;

  soap_env_add_itemf(env,"xsd:string", "echo", 
		     "Hello '%s'", name);

  xmlFree((xmlChar*)name);

}


SoapEnv* say_hello(SoapEnv *request)
{

  SoapEnv *env;
  xmlNodePtr method, node;

  env = soap_env_new_with_response(request);

  method = soap_env_get_method(request);
  node = soap_xml_get_children(method);

  while (node) {
    add_name(node, env);
    node = soap_xml_get_next(node);
  }

  return env;
}


int main(int argc, char *argv[])
{

  SoapRouter *router;
  
  log_set_level(HLOG_VERBOSE);

  if (!soap_server_init_args(argc, argv)) {
    return 0;
  }
  
  router = soap_router_new();
  soap_router_register_service(router, say_hello, method, urn);
  soap_server_register_router(router, url);

  log_info1("send SIGTERM to shutdown");
  soap_server_run();

  log_info1("shutting down\n"); 
  soap_server_destroy();

  return 0;
}





