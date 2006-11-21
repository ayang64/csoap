/******************************************************************
*  $Id: soap-transport.h,v 1.1 2006/11/21 20:59:02 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2007 Heiko Ronsdorf
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
* Email: hero@persua.de
******************************************************************/
#ifndef __csoap_transport_h
#define __csoap_transport_h

#ifdef __CSOAP_INTERNAL
typedef herror_t (*msg_exchange)(void *data, SoapCtx *request, SoapCtx **response);

#ifdef __cplusplus
extern "C" {
#endif

extern herror_t soap_transport_server_init_args(int argc, char **argv);
extern herror_t soap_transport_server_run(void);
extern void soap_transport_server_destroy(void);

extern const char *soap_transport_get_name(void);
extern herror_t soap_transport_add(const char *protocol, void *data, msg_exchange invoke);
extern herror_t soap_transport_remove(const char *protocol, void *data);
extern herror_t soap_transport_register_router(SoapRouter *router, const char *context);

extern herror_t soap_transport_send(SoapCtx *request, SoapCtx **response);
extern herror_t soap_transport_receive(SoapCtx *request, SoapCtx **response);

extern herror_t soap_transport_client_init_args(int argc, char **argv);
extern herror_t soap_transport_client_invoke(SoapCtx *req, SoapCtx **res);
extern void soap_transport_client_destroy(void);

#ifdef __cplusplus
}
#endif

#endif

#endif
