/******************************************************************
 *  $Id: soap-server.h,v 1.12 2006/11/21 20:59:02 m0gg Exp $
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
 * Register a router to the soap server. 
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
extern herror_t soap_server_register_router(SoapRouter * router, const char *context);

extern const char *soap_server_get_name(void);

/**
 *
 * Enters the server loop and starts to listen to incoming requests.
 *
 * @see httpd_run
 * @see udpd_run
 *
 */
extern herror_t soap_server_run(void);

/**
 *
 * Frees the soap server.
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
  SoapRouter *router;
  struct _SoapRouterNode *next;

} SoapRouterNode;

extern SoapRouter *soap_server_find_router(const char *context);

extern SoapRouterNode * soap_server_get_routers(void);

extern herror_t soap_server_process(SoapCtx *request, SoapCtx **response);

#endif

#ifdef __cplusplus
}
#endif

#endif
