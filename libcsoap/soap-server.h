/******************************************************************
 *  $Id: soap-server.h,v 1.27 2007/01/25 10:28:30 m0gg Exp $
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
 * @section project_intro_sec Introduction
 *
 * cSOAP is a client/server SOAP library implemented in pure C. It comes with
 * embedded transport servers for UDP and HTTP (nanoHTTP). The transferred XML
 * structures are handled by libxml2.
 *
 * @section project_features_sec Features
 *
 * - SOAP 1.1 compliant (@subpage csoap_page)
 * - supports different transport services
 *   - HTTP including SSL (@subpage nanohttp_page)
 *   - client/server UDP transport service (multicast, SOAP-over-UDP)
 * - attachments via MIME
 * - message based security (XML encryption/signation)
 * - automatic generation of a WS-Inspection document
 * - Platform independent: Windows, Unix, Linux, MacOS, OpenVMS, AIX
 * - Written in pure ANSI C (compiles with gcc -ansi).
 *
 * @section howto_sec HOWTOs and coding examples
 *
 * - @ref nanohttp_client_page
 * - @ref nanohttp_server_page
 * - @ref nanohttp_mime_page
 * - @ref csoap_client_page
 * - @ref csoap_server_page
 * - @ref csoap_mime_page
 * - @ref csoap_xmlsec_page
 *
 * @section help_sec How to help
 *
 * The development of cSOAP highly depends on your input! If you are trying
 * cSOAP let us know what you think of it (do you miss certain features?). Even
 * if you decide not to use it, please let us know why.
 *
 * @section projects_sec Projects that use cSOAP
 *
 * @subsection complearn_sec CompLearn
 *
 * CompLearn (http://complearn.org/) is a suite of simple-to-use utilities that
 * you can use to apply compression techniques to the process of discovering and
 * learning patterns.
 *
 * @section downloads_sec Downloads
 *
 * @subsection download_stable_sec Download stable release
 *
 * A stable release is a tested version of cSOAP. Download this package if you
 * want to use it in your productive projects. 
 *
 * Download source code of libsoap-1.1.0.tar.gz:
 * http://prdownloads.sourceforge.net/csoap/libsoap-1.1.0.tar.gz?download
 *
 * @subsection download_snapshot_sec Download latest snapshot (nightly build)
 *
 * Latest snapshots are build nightly on a detected CVS commit. This is sometimes
 * not very stable but in some case are snapshots the better choice then the
 * stable version. This can happen if a very important bug was fixed or a new
 * feature was introduced. It is recommend to join the mailinglist.
 *
 * Download latest snapshot (nightly build):
 * http://csoap.sourceforge.net/downloads/libsoap-snapshot.tar.gz
 *
 * @subsection download_cvs_sec Checkout from CVS
 *
 * Do you want the latest source codes? You want to contribute a patch? Have you
 * found a bug? Or whatever. You should check out cSOAP from CVS to play the game
 * with us cSOAP developers.
 *
 * Web CVS access: http://csoap.cvs.sourceforge.net/csoap/libsoap/
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
 *
 */

/** @page csoap_page cSOAP
 *
 * @section csoap_sec cSOAP
 *
 * cSOAP is a C-based implementation of the Simple Object Access Protocol (SOAP)
 * version 1.1.
 *
 * SOAP is a lightweight protocol for exchange of information in a decentralized,
 * distributed environment. It is an XML based protocol that consists of three
 * parts:
 * - an envelope that defines a framework for describing what is in a message and
 *   how to process it,
 * - a set of encoding rules for expressing instances of application-defined
 *   datatypes,
 * - and a convention for representing remote procedure calls and responses.
 * SOAP can potentially be used in combination with a variety of other protocols;
 * however, the only bindings defined in this document describe how to use SOAP
 * in combination with HTTP and HTTP Extension Framework.
 *
 * @section soap_howto_sec HOWTOs and coding examples
 *
 * - @subpage csoap_client_page
 * - @subpage csoap_server_page
 * - @subpage csoap_mime_page
 * - @subpage csoap_xmlsec_page
 *
 * @version	1.2
 *
 * @author	Ferhat Ayaz
 * @author	Michael Rans
 * @author	Matt Campbell
 * @author	Heiko Ronsdorf
 *
 * @see		http://www.w3.org/TR/soap/
 * @see		http://www.w3.org/TR/2000/NOTE-SOAP-20000508/
 *
 */

/** @page csoap_server_page Howto write a SOAP server
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
 * static const char const *url = "/csoapserver";
 * static const char const *urn = "urn:examples";
 * static const char const *method = "sayHello";
 *
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
 *   if ((err = soap_env_new_with_response(req->env, &res->env)) != H_OK)
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
