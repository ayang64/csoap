/******************************************************************
 *  $Id: soap-client.h,v 1.14 2006/11/23 15:27:33 m0gg Exp $
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

#define SOAP_ERROR_CLIENT_INIT 5001

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
 *  @soap_action value for "SoapAction:" in the HTTP request header.
 *
 * @returns H_OK if success 
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
