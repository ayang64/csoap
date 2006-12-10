/******************************************************************
 *  $Id: soap-server.h,v 1.20 2006/12/10 19:21:06 m0gg Exp $
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef __csoap_server_h
#define __csoap_server_h

/**
 *
 * @mainpage Project overview
 *
 * @section seq_intro Introduction
 *
 * cSOAP is a client/server SOAP library implemented in pure C. It comes with
 * embedded transport servers for UDP and HTTP (nanohttp). The transferred XML
 * structures are handled by libxml2.
 *
 * @section seq_features Features
 *
 * - different transport services
 *   - @ref nanohttp_page (HTTP including SSL)
 *   - client/server UDP transport service (multicast)
 * - attachments via MIME
 * - message based security (XML encryption/signation)
 * - automatic generation of WS-Inspection
 *
 * @author	Ferhat Ayaz
 * @author	Michael Rans
 * @author	Matt Campbell
 * @author	Heiko Ronsdorf
 *
 * @version	1.2
 *
 * @see		http://csoap.sourceforge.net/
 * @see		http://www.libxml.org/
 * @see		http://www.openssl.org/
 * @see		http://www.aleksey.com/xmlsec/
 */

/** @page soap_server_page Howto write an SOAP server
 *
 * @section soap_server_toc_sec Table of contents
 *
 * - @ref soap_server_init_sec
 * - @ref soap_server_router_sec
 * - @ref soap_server_registration_sec
 * - @ref soap_server_running_sec
 * - @ref soap_server_cleanup_sec
 * - @ref soap_server_function_sec
 *
 * @section soap_server_init_sec Server initialization
 *
 * @code
 * int main(int argc, char **argv)
 * {
 *   herror_t err;
 *   struct SoapRouter *router;
 *
 *   if ((err = soap_server_init_args(argc, argv)) != H_OK)
 *   {
 *     printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     exit(1);
 *   }
 * @endcode
 *
 * @section soap_server_router_sec Router creation
 *
 * @code  
 *   if (!(router = soap_router_new()))
 *   {
 *     printf("soap_router_new failed (router == %p)\n", router);
 *     herror_release(err);
 *     exit(1);
 *   }
 * @endcode
 *
 * @section soap_server_registration_sec Service registration
 *
 * @code
 *   if ((err = soap_router_register_service(router, say_hello, method, urn)) != H_OK)
 *   {
 *     printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     exit(1);
 *   }
 * @endcode
 *
 * @code
 *   if ((err = soap_server_register_router(router, url)))
 *   {
 *     printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     exit(1);
 *   }
 *   printf("router (%p) registered for \"%s\"\n", router, url);
 * @endcode
 *
 * @section soap_server_running_sec Serving requests
 *
 * @code
 *   printf("press ctrl-c to shutdown\n");
 *   if ((err = soap_server_run()) != H_OK)
 *   {
 *     printf("%s(): %s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     exit(1);
 *   }
 * @endcode
 *
 * @section soap_server_cleanup_sec Server cleanup
 *
 * @code
 *   soap_server_destroy();
 *
 *   exit(0);
 * }
 * @endcode
 *
 * @section soap_server_function_sec Service function
 *
 * @code
 * herror_t say_hello(struct SoapCtx *req, struct SoapCtx *res)
 * {
 *   herror_t err;
 *   char *name;
 *   xmlNodePtr method, node;
 *
 *   printf("processing service request\n");
 *
 *   xmlDocFormatDump(stdout, req->env->root->doc, 1);
 *
 *   err = soap_env_new_with_response(req->env, &res->env);
 *   if (err != H_OK)
 *   {
 *     printf("soap_env_new_with_response failed (%s)\n", herror_message(err));
 *     return err;
 *   }
 *   printf("empty response created\n");
 *
 *   if (!(method = soap_env_get_method(req->env)))
 *   {
 *     printf("soap_env_get_method failed\n");
 *     return herror_new("say_hello", 0, "There may be a bug in the library...");
 *   }
 *   printf("method found\n");
 *
 *   printf("adding response content...\n");
 *   node = soap_xml_get_children(method);
 *
 *   while (node)
 *   {
 *     name = (char *) xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
 *     soap_env_add_itemf(res->env, "xsd:string", "echo", "Hello '%s'", name);
 *     node = soap_xml_get_next(node);
 *     if (name)
 *       xmlFree(name);
 *   }
 *   printf("service request done\n");
 *   return H_OK;
 * }
 * @endcode
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the soap server with commandline arguments.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 * @see httpd_init_args
 * @see udpd_init_args
 *
 */
herror_t soap_server_init_args(int argc, char **argv);

/**
 *
 * Register a router to the SOAP server. 
 *
 * <p><i>scheme</i>://<i>host</i>:<i>port</i>/<b>[context]</b>
 *
 * @param router The router to register
 * @param context the url context 
 *
 * @returns H_OK on success
 * 
 * @see soap_router_new
 * @see soap_router_register_service
 *
 */
extern herror_t soap_server_register_router(struct SoapRouter * router, const char *context);

/**
 *
 * Returns an URI name of the server.
 *
 */
extern const char *soap_server_get_name(void);

/**
 *
 * Enters the server loop and starts to listen to incoming requests.
 *
 * @see httpd_run
 * @see udpd_run
 * @see smptd_run
 *
 */
extern herror_t soap_server_run(void);

/**
 *
 * Frees the SOAP server.
 *
 * @see httpd_destroy
 * @see udpd_destroy
 *
 */
extern void soap_server_destroy(void);

#ifdef __CSOAP_INTERNAL

typedef struct _SoapRouterNode
{
  char *context;
  struct SoapRouter *router;
  struct _SoapRouterNode *next;

} SoapRouterNode;

extern struct SoapRouter *soap_server_find_router(const char *context);

extern SoapRouterNode * soap_server_get_routers(void);

extern herror_t soap_server_process(struct SoapCtx *request, struct SoapCtx **response);

#endif

#ifdef __cplusplus
}
#endif

#endif
