/******************************************************************
 * $Id: simpleclient.c,v 1.4 2004/08/30 15:26:48 snowdrop Exp $
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

#include <libcsoap/soap-client.h>


/*
static const char *url = "http://csoap.sourceforge.net/cgi-bin/csoapserver";
*/
static const char *url = "http://localhost:10000/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";


int main(int argc, char *argv[])
{
  SoapEnv *env, *res;

  log_set_level(HLOG_VERBOSE);
  if (!soap_client_init_args(argc, argv)) {
	  return 1;
  }

  env = soap_env_new_with_method(urn, method);
  soap_env_add_item(env, "xsd:string", "name", "Jonny B. Good");

  if (argc > 1)
    res = soap_client_invoke(env, argv[1], "");
  else
    res = soap_client_invoke(env, url, "");
  soap_xml_doc_print(res->root->doc);
  soap_env_free(res);
  soap_env_free(env);

  return 0;
}





