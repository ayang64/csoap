/******************************************************************
*  $Id: soap-nudp.h,v 1.3 2006/11/26 20:13:05 m0gg Exp $
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
#ifndef __csoap_nudp_h
#define __csoap_nudp_h

#ifdef __CSOAP_INTERNAL

#define NUDP_ARG_PORT		"-NUDPport"

#define NUDP_DEFAULT_PORT	10001

#ifdef __cplusplus
extern "C" {
#endif

extern herror_t soap_nudp_server_init_args(int argc, char **argv);
extern void *soap_nudp_server_run(void *unused);
extern herror_t soap_nudp_server_run_threaded(void);
extern void soap_nudp_server_destroy(void);

extern herror_t soap_nudp_register(const void *data);

extern herror_t soap_nudp_client_init_args(int argc, char **argv);
extern void soap_nudp_client_destroy(void);

#ifdef __cplusplus
}
#endif

#endif

#endif
