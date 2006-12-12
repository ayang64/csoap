/******************************************************************
 *  $Id: soap-client.h,v 1.18 2006/12/12 07:23:11 m0gg Exp $
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
#ifndef __csoap_client_h
#define __csoap_client_h

#ifndef __CSOAP_INTERNAL
#include <nanohttp/nanohttp-client.h>

#include <libcsoap/soap-ctx.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-addressing.h>
#endif

/** @page csoap_client_page Howto write a SOAP client
 *
 * @section toc_sec Table of contents
 *
 * - @ref soap_client_init
 * - @ref envelope_sec
 * - @ref invoke_sec
 * - @ref result_sec
 * - @ref soap_request_cleanup_sec
 * - @ref soap_client_cleanup_sec
 *
 * @section soap_client_init Client initialization
 *
 * @code
 * static char *url = "http://localhost:10000/csoapserver";
 * static char *urn = "urn:examples";
 * static char *method = "sayHello";
 *
 * int * main(int argc, char **argv)
 * {
 *   struct SoapCtx *request;
 *   struct SoapCtx *response;
 *   herror_t err;
 *
 *   if ((err = soap_client_init_args(argc, argv)) != H_OK)
 *   {
 *     printf("%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     exit(1);
 *   }
 * @endcode
 *
 * @section envelope_sec Envelope creation
 *
 * @code
 *   if ((err = soap_ctx_new_with_method(urn, method, &request)) != H_OK)
 *   {
 *     printf("%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
 *     herror_release(err);
 *     soap_client_destroy();
 *     exit(1);
 *   }
 *
 *   soap_env_add_item(request->env, "xsd:string", "name", "Jonny B. Good");
 * @endcode
 *
 * @section invoke_sec Invocation
 *
 * @code
 *   if ((err = soap_client_invoke(request, &response, url, "")) != H_OK)
 *   {
 *     printf("[%d] %s(): %s\n", herror_code(err), herror_func(err), herror_message(err));
 *     herror_release(err);
 *     soap_ctx_free(request);
 *     soap_client_destroy();
 *     exit(1);
 *   }
 * @endcode
 *
 * @section result_sec Printout result
 *
 * @code
 *   printf("**** received from \"%s\" ****\n", soap_addressing_get_from_address_string(response->env));
 *   xmlDocFormatDump(stdout, response->env->root->doc, 1);
 * @endcode
 *
 * @section soap_request_cleanup_sec Request cleanup
 *
 * @code
 *   soap_ctx_free(response);
 *   soap_ctx_free(request);
 * @endcode
 *
 * @section soap_client_cleanup_sec Client cleanup
 *
 * @code
 *   soap_client_destroy();
 *
 *   exit(0);
 * }
 * @endcode
 *
 */


#define SOAP_ERROR_CLIENT		5000
#define SOAP_ERROR_CLIENT_GENERIC	(SOAP_ERROR_CLIENT + 0)
#define SOAP_ERROR_CLIENT_INIT		(SOAP_ERROR_CLIENT + 1)

/**
 *
 * Commandline argument to force encryption of outgoing messages.
 *
 * @see soap_xmlsec_encrypt
 *
 */
#define CSOAP_CLIENT_FORCE_ENCRYPT	"-CSOAPencrypt"

/**
 *
 * Commandline argument to force decryption of incoming messages.
 *
 * @see soap_xmlsec_decrypt
 *
 */
#define CSOAP_CLIENT_FORCE_DECRYPT	"-CSOAPdecrypt"

/**
 *
 * Commandline argument to force signature on outgoing messages.
 *
 * @see soap_xmlsec_sign
 *
 */
#define CSOAP_CLIENT_FORCE_SIGN		"-CSOAPsign"

/**
 *
 * Commandline argument to force signature on incoming messages.
 *
 * @see soap_xmlsec_verify
 *
 */
#define CSOAP_CLIENT_FORCE_VERIFY	"-CSOAPverify"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the client side soap engine
 *
 */
extern herror_t soap_client_init_args(int argc, char **argv);

/**
 *
 *  Establish connection to the soap server and send the given envelope. 
 * 
 *  @param env envelope to send
 *  @param response  the result envelope
 *  @param url url to the soap server
 *
 *  @param soap_action value for "SoapAction:" in the HTTP request header.
 *
 *  @return H_OK if success 
 *
 */
extern herror_t soap_client_invoke(struct SoapCtx * ctx, struct SoapCtx **response, const char *url, const char *soap_action);

/**
 *
 * Destroy the soap client module
 *
 */
extern void soap_client_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
